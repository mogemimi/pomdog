// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/linux/gamepad_linux.h"
#include "pomdog/input/backends/gamepad_helper.h"
#include "pomdog/logging/log.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <algorithm>
#include <cstring>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::linux {
namespace {

[[nodiscard]] constexpr size_t bitCount(size_t n) noexcept
{
    return (n / 8) + 1;
}

template <typename T>
[[nodiscard]] bool hasBit(const T& array, uint32_t bit) noexcept
{
    return ((1 << (bit % 8)) & array[bit / 8]) != 0;
}

[[nodiscard]] float normalizeAxisValue(int value, const ThumbStickInfo& info)
{
    return static_cast<float>(info.invertDirection * ((value - info.minimum) * 2 - info.range)) / info.range;
}

} // namespace

bool GamepadDevice::open(int deviceIndex)
{
    POMDOG_ASSERT(fd == -1);

    const auto js = "/dev/input/event" + std::to_string(deviceIndex);
    fd = ::open(js.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        fd = -1;
        return false;
    }

    POMDOG_ASSERT(fd >= 0);

    struct input_id inputId;
    if (::ioctl(fd, EVIOCGID, &inputId) < 0) {
        close();
        return false;
    }

    auto& uuid = caps.deviceUUID;
    uuid.busType = static_cast<uint16_t>(inputId.bustype);
    uuid.vendorID = static_cast<uint16_t>(inputId.vendor);
    uuid.productID = static_cast<uint16_t>(inputId.product);
    uuid.versionNumber = static_cast<uint16_t>(inputId.version);

    // TODO: Use udev-joystick-blacklist here
    // https://github.com/denilsonsa/udev-joystick-blacklist

    std::array<std::uint8_t, bitCount(EV_MAX)> evBits;
    std::array<std::uint8_t, bitCount(KEY_MAX)> keyBits;
    std::array<std::uint8_t, bitCount(ABS_MAX)> absBits;

    std::fill(std::begin(evBits), std::end(evBits), 0);
    std::fill(std::begin(keyBits), std::end(keyBits), 0);
    std::fill(std::begin(absBits), std::end(absBits), 0);

    if ((ioctl(fd, EVIOCGBIT(0, evBits.size()), evBits.data()) < 0) ||
        (ioctl(fd, EVIOCGBIT(EV_KEY, keyBits.size()), keyBits.data()) < 0) ||
        (ioctl(fd, EVIOCGBIT(EV_ABS, absBits.size()), absBits.data()) < 0)) {
        close();
        return false;
    }

    if (!hasBit(evBits, EV_KEY) ||
        !hasBit(evBits, EV_ABS) ||
        !hasBit(absBits, ABS_X) ||
        !hasBit(absBits, ABS_Y)) {
        // The device is not a joystick or gamepad.
        close();
        return false;
    }

    std::tie(this->mappings, this->caps.name) = gamepad_mappings::getMappings(uuid);

    int numOfButtons = 0;
    std::fill(std::begin(keyMap), std::end(keyMap), -1);

    for (int i = BTN_JOYSTICK; i < KEY_MAX; ++i) {
        if (hasBit(keyBits, static_cast<uint32_t>(i))) {
            const auto index = i - BTN_JOYSTICK;
            if ((index < 0) || (index >= static_cast<int>(keyMap.size()))) {
                continue;
            }
            keyMap[index] = static_cast<int8_t>(numOfButtons);

            POMDOG_ASSERT(numOfButtons >= 0);
            POMDOG_ASSERT(numOfButtons < static_cast<int>(mappings.buttons.size()));

            if (auto hasButton = gamepad_mappings::hasButton(caps, mappings.buttons, numOfButtons); hasButton != nullptr) {
                (*hasButton) = true;
            }
            ++numOfButtons;
        }
    }

    static_assert(ABS_X == 0, "");
    static_assert(ABS_RZ == 5, "");

    for (int i = ABS_X; i <= ABS_RZ; ++i) {
        if (hasBit(absBits, i)) {
            struct input_absinfo absInfo;
            if (::ioctl(fd, EVIOCGABS(i), &absInfo) < 0) {
                continue;
            }

            POMDOG_ASSERT(i >= 0);
            POMDOG_ASSERT(i < static_cast<int>(mappings.axes.size()));

            const auto& mapper = mappings.axes[i];

            if (auto hasThumbStick = gamepad_mappings::hasThumbStick(caps, mapper.thumbStick); hasThumbStick != nullptr) {
                (*hasThumbStick) = true;
            }
            if (auto hasButton = gamepad_mappings::hasButton(caps, mapper.positiveTrigger); hasButton != nullptr) {
                (*hasButton) = true;
            }
            if (auto hasButton = gamepad_mappings::hasButton(caps, mapper.negativeTrigger); hasButton != nullptr) {
                (*hasButton) = true;
            }

            POMDOG_ASSERT(i >= 0);
            POMDOG_ASSERT(i < static_cast<int>(thumbStickInfos.size()));
            auto& info = thumbStickInfos[i];
            info.minimum = absInfo.minimum;
            info.range = std::max(1, absInfo.maximum - absInfo.minimum);

            switch (mapper.thumbStick) {
            case ThumbStickKind::LeftStickY:
            case ThumbStickKind::RightStickY:
                // Set to -1 to reverse the Y axis (vertical)
                info.invertDirection = -1;
                break;
            default:
                info.invertDirection = 1;
                break;
            }
        }
    }

    if (caps.name.empty()) {
        std::array<char, 256> gamepadName;
        std::fill(std::begin(gamepadName), std::end(gamepadName), 0);
        if (::ioctl(fd, EVIOCGNAME(255), gamepadName.data()) < 0) {
            close();
            return false;
        }
        caps.name = gamepadName.data();
    }

    // NOTE: gamepad is open.
    GamepadHelper::clearState(state);
    state.isConnected = true;

    this->deviceEventIndex = deviceIndex;

    return true;
}

void GamepadDevice::close()
{
    if (fd >= 0) {
        ::close(fd);
        fd = -1;
    }

    GamepadHelper::clearState(state);
    state.isConnected = false;

    GamepadCapabilities emptyCaps;
    std::swap(caps, emptyCaps);
}

bool GamepadDevice::hasFileDescriptor() const
{
    return fd >= 0;
}

bool GamepadDevice::pollEvents()
{
    for (;;) {
        struct input_event event;

        errno = 0;
        if (::read(fd, &event, sizeof(event)) < 0) {
            if (errno == ENODEV) {
                // NOTE: gamepad is disconnected.
                return false;
            }
            break;
        }

        switch (event.type) {
        case EV_KEY: {
            const auto physicalIndex = static_cast<int>(event.code) - BTN_JOYSTICK;
            if ((physicalIndex < 0) || (physicalIndex >= static_cast<int>(keyMap.size()))) {
                break;
            }
            const auto buttonIndex = keyMap[physicalIndex];
            if (auto button = gamepad_mappings::getButton(state, mappings.buttons, buttonIndex); button != nullptr) {
                (*button) = (event.value != 0) ? ButtonState::Pressed : ButtonState::Released;
            }
            break;
        }
        case EV_ABS:
            switch (event.code) {
            case ABS_HAT0X:
            case ABS_HAT0Y:
            case ABS_HAT1X:
            case ABS_HAT1Y:
            case ABS_HAT2X:
            case ABS_HAT2Y:
            case ABS_HAT3X:
            case ABS_HAT3Y: {
                const auto code = (event.code - ABS_HAT0X);
                const auto axis = code % 2;
                if (event.value == 0) {
                    if (axis == 0) {
                        state.dpad.left = ButtonState::Released;
                        state.dpad.right = ButtonState::Released;
                    }
                    else {
                        state.dpad.up = ButtonState::Released;
                        state.dpad.down = ButtonState::Released;
                    }
                }
                else if (event.value > 0) {
                    if (axis == 0) {
                        state.dpad.right = ButtonState::Pressed;
                    }
                    else {
                        state.dpad.down = ButtonState::Pressed;
                    }
                }
                else if (event.value < 0) {
                    if (axis == 0) {
                        state.dpad.left = ButtonState::Pressed;
                    }
                    else {
                        state.dpad.up = ButtonState::Pressed;
                    }
                }
                break;
            }
            case ABS_X:
            case ABS_Y:
            case ABS_Z:
            case ABS_RX:
            case ABS_RY:
            case ABS_RZ: {
                static_assert(ABS_X == 0, "");
                static_assert(ABS_Y == 1, "");
                static_assert(ABS_Z == 2, "");
                static_assert(ABS_RX == 3, "");
                static_assert(ABS_RY == 4, "");
                static_assert(ABS_RZ == 5, "");
                POMDOG_ASSERT(event.code >= 0);
                POMDOG_ASSERT(event.code < static_cast<int>(mappings.axes.size()));
                POMDOG_ASSERT(event.code < static_cast<int>(thumbStickInfos.size()));

                const auto& mapper = mappings.axes[event.code];

                if (auto thumbStick = gamepad_mappings::getThumbStick(state, mapper.thumbStick); thumbStick != nullptr) {
                    (*thumbStick) = normalizeAxisValue(event.value, thumbStickInfos[event.code]);
                }

                if (auto button = gamepad_mappings::getButton(state, mapper.positiveTrigger); button != nullptr) {
                    const auto value = normalizeAxisValue(event.value, thumbStickInfos[event.code]);
                    constexpr float threshold = 0.05f;
                    (*button) = (value > threshold) ? ButtonState::Pressed : ButtonState::Released;
                }

                if (auto button = gamepad_mappings::getButton(state, mapper.negativeTrigger); button != nullptr) {
                    const auto value = normalizeAxisValue(event.value, thumbStickInfos[event.code]);
                    constexpr float threshold = -0.05f;
                    (*button) = (value < threshold) ? ButtonState::Pressed : ButtonState::Released;
                }
                break;
            }
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    return true;
}

GamepadLinux::GamepadLinux() noexcept
{
    gamepads_[0].playerIndex = PlayerIndex::One;
    gamepads_[1].playerIndex = PlayerIndex::Two;
    gamepads_[2].playerIndex = PlayerIndex::Three;
    gamepads_[3].playerIndex = PlayerIndex::Four;
}

GamepadLinux::~GamepadLinux()
{
    for (auto& gamepad : gamepads_) {
        if (gamepad.hasFileDescriptor()) {
            gamepad.close();
        }
    }
}

GamepadCapabilities
GamepadLinux::getCapabilities(PlayerIndex playerIndex) const
{
    const auto index = GamepadHelper::toInt(playerIndex);
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(gamepads_.size()));
    return gamepads_[index].caps;
}

GamepadState
GamepadLinux::getState(PlayerIndex playerIndex) const
{
    const auto index = GamepadHelper::toInt(playerIndex);
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(gamepads_.size()));
    return gamepads_[index].state;
}

void GamepadLinux::enumerateDevices()
{
    auto gamepad = std::begin(gamepads_);

    for (int i = 0; i < 32; i++) {
        while ((gamepad != std::end(gamepads_)) && gamepad->hasFileDescriptor()) {
            ++gamepad;
        }
        if (gamepad == std::end(gamepads_)) {
            break;
        }

        auto iter = std::find_if(std::begin(gamepads_), std::end(gamepads_), [&](auto& device) {
            return (device.hasFileDescriptor() && (device.deviceEventIndex == i));
        });
        if (iter != std::end(gamepads_)) {
            // The device is already opened.
            continue;
        }

        POMDOG_ASSERT(!gamepad->hasFileDescriptor());

        if (!gamepad->open(i)) {
            gamepad->close();
            continue;
        }

        this->Connected(gamepad->playerIndex, gamepad->caps);
    }
}

void GamepadLinux::pollEvents()
{
    for (auto& gamepad : gamepads_) {
        if (!gamepad.hasFileDescriptor()) {
            continue;
        }

        if (!gamepad.pollEvents()) {
            auto caps = gamepad.caps;
            gamepad.close();
            this->Disconnected(gamepad.playerIndex, caps);
        }
    }
}

} // namespace pomdog::detail::linux
