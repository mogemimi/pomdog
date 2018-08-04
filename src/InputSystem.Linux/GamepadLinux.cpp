// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "GamepadLinux.hpp"
#include "../InputSystem/GamepadHelper.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <algorithm>
#include <cstring>
#include <tuple>

namespace Pomdog {
namespace Detail {
namespace InputSystem {
namespace Linux {
namespace {

constexpr size_t BitCount(size_t n)
{
    return (n / 8) + 1;
}

template <typename T>
bool HasBit(const T& array, uint32_t bit)
{
    return ((1 << (bit % 8)) & array[bit / 8]) != 0;
}

float NormalizeAxisValue(int value, const ThumbStickInfo& info)
{
    return static_cast<float>(info.InvertDirection * ((value - info.Minimum) * 2 - info.Range)) / info.Range;
}

} // unnamed namespace

bool GamepadDevice::Open(int deviceIndex)
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
        Close();
        return false;
    }

    auto& uuid = caps.DeviceUUID;
    uuid.BusType = static_cast<uint16_t>(inputId.bustype);
    uuid.VendorID = static_cast<uint16_t>(inputId.vendor);
    uuid.ProductID = static_cast<uint16_t>(inputId.product);
    uuid.VersionNumber = static_cast<uint16_t>(inputId.version);

    // TODO: Use udev-joystick-blacklist here
    // https://github.com/denilsonsa/udev-joystick-blacklist

    std::array<uint8_t, BitCount(EV_MAX)> evBits;
    std::array<uint8_t, BitCount(KEY_MAX)> keyBits;
    std::array<uint8_t, BitCount(ABS_MAX)> absBits;

    std::fill(std::begin(evBits), std::end(evBits), 0);
    std::fill(std::begin(keyBits), std::end(keyBits), 0);
    std::fill(std::begin(absBits), std::end(absBits), 0);

    if ((ioctl(fd, EVIOCGBIT(0, evBits.size()), evBits.data()) < 0) ||
        (ioctl(fd, EVIOCGBIT(EV_KEY, keyBits.size()), keyBits.data()) < 0) ||
        (ioctl(fd, EVIOCGBIT(EV_ABS, absBits.size()), absBits.data()) < 0)) {
        Close();
        return false;
    }

    if (!HasBit(evBits, EV_KEY) ||
        !HasBit(evBits, EV_ABS) ||
        !HasBit(absBits, ABS_X) ||
        !HasBit(absBits, ABS_Y)) {
        // The device is not a joystick or gamepad.
        Close();
        return false;
    }

    std::tie(this->mappings, this->caps.Name) = GetMappings(uuid);

    int numOfButtons = 0;
    std::fill(std::begin(keyMap), std::end(keyMap), -1);

    for (int i = BTN_JOYSTICK; i < KEY_MAX; ++i) {
        if (HasBit(keyBits, static_cast<uint32_t>(i))) {
            const auto index = i - BTN_JOYSTICK;
            if ((index < 0) || (index >= static_cast<int>(keyMap.size()))) {
                continue;
            }
            keyMap[index] = static_cast<int8_t>(numOfButtons);

            POMDOG_ASSERT(numOfButtons >= 0);
            POMDOG_ASSERT(numOfButtons < static_cast<int>(mappings.buttons.size()));

            if (auto hasButton = HasButton(caps, mappings.buttons, numOfButtons); hasButton != nullptr) {
                (*hasButton) = true;
            }
            ++numOfButtons;
        }
    }

    static_assert(ABS_X == 0, "");
    static_assert(ABS_RZ == 5, "");

    for (int i = ABS_X; i <= ABS_RZ; ++i) {
        if (HasBit(absBits, i)) {
            struct input_absinfo absInfo;
            if (::ioctl(fd, EVIOCGABS(i), &absInfo) < 0) {
                continue;
            }

            POMDOG_ASSERT(i >= 0);
            POMDOG_ASSERT(i < static_cast<int>(mappings.axes.size()));

            const auto& mapper = mappings.axes[i];

            if (auto hasThumbStick = HasThumbStick(caps, mapper.thumbStick); hasThumbStick != nullptr) {
                (*hasThumbStick) = true;
            }
            if (auto hasButton = HasButton(caps, mapper.positiveTrigger); hasButton != nullptr) {
                (*hasButton) = true;
            }
            if (auto hasButton = HasButton(caps, mapper.negativeTrigger); hasButton != nullptr) {
                (*hasButton) = true;
            }

            POMDOG_ASSERT(i >= 0);
            POMDOG_ASSERT(i < static_cast<int>(thumbStickInfos.size()));
            auto& info = thumbStickInfos[i];
            info.Minimum = absInfo.minimum;
            info.Range = std::max(1, absInfo.maximum - absInfo.minimum);

            switch (mapper.thumbStick) {
            case ThumbStickKind::LeftStickY:
            case ThumbStickKind::RightStickY:
                // Set to -1 to reverse the Y axis (vertical)
                info.InvertDirection = -1;
                break;
            default:
                info.InvertDirection = 1;
                break;
            }
        }
    }

    if (caps.Name.empty()) {
        std::array<char, 256> gamepadName;
        std::fill(std::begin(gamepadName), std::end(gamepadName), 0);
        if (::ioctl(fd, EVIOCGNAME(255), gamepadName.data()) < 0) {
            Close();
            return false;
        }
        caps.Name = gamepadName.data();
    }

    GamepadHelper::ClearState(state);
    state.IsConnected = true;

    this->deviceEventIndex = deviceIndex;

    Log::Internal("Open gamepad: " + caps.Name + " at " + js);
    return true;
}

void GamepadDevice::Close()
{
    if (fd >= 0) {
        ::close(fd);
        fd = -1;
    }

    GamepadHelper::ClearState(state);
    state.IsConnected = false;

    GamepadCapabilities emptyCaps;
    std::swap(caps, emptyCaps);
}

bool GamepadDevice::HasFileDescriptor() const
{
    return fd >= 0;
}

bool GamepadDevice::PollEvents()
{
    for (;;) {
        struct input_event event;

        errno = 0;
        if (::read(fd, &event, sizeof(event)) < 0) {
            if (errno == ENODEV) {
                Log::Internal("Disconnect gamepad: " + caps.Name);
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
            if (auto button = GetButton(state, mappings.buttons, buttonIndex); button != nullptr) {
                (*button) = (event.value != 0) ? ButtonState::Pressed: ButtonState::Released;
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
                        state.DPad.Left = ButtonState::Released;
                        state.DPad.Right = ButtonState::Released;
                    }
                    else {
                        state.DPad.Up = ButtonState::Released;
                        state.DPad.Down = ButtonState::Released;
                    }
                }
                else if (event.value > 0) {
                    if (axis == 0) {
                        state.DPad.Right = ButtonState::Pressed;
                    }
                    else {
                        state.DPad.Down = ButtonState::Pressed;
                    }
                }
                else if (event.value < 0) {
                    if (axis == 0) {
                        state.DPad.Left = ButtonState::Pressed;
                    }
                    else {
                        state.DPad.Up = ButtonState::Pressed;
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

                if (auto thumbStick = GetThumbStick(state, mapper.thumbStick); thumbStick != nullptr) {
                    (*thumbStick) = NormalizeAxisValue(event.value, thumbStickInfos[event.code]);
                }

                if (auto button = GetButton(state, mapper.positiveTrigger); button != nullptr) {
                    const auto value = NormalizeAxisValue(event.value, thumbStickInfos[event.code]);
                    constexpr float threshold = 0.05f;
                    (*button) = (value > threshold) ? ButtonState::Pressed : ButtonState::Released;
                }

                if (auto button = GetButton(state, mapper.negativeTrigger); button != nullptr) {
                    const auto value = NormalizeAxisValue(event.value, thumbStickInfos[event.code]);
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

GamepadLinux::GamepadLinux()
{
    gamepads[0].playerIndex = PlayerIndex::One;
    gamepads[1].playerIndex = PlayerIndex::Two;
    gamepads[2].playerIndex = PlayerIndex::Three;
    gamepads[3].playerIndex = PlayerIndex::Four;
}

GamepadLinux::~GamepadLinux()
{
    for (auto& gamepad : gamepads) {
        if (gamepad.HasFileDescriptor()) {
            Log::Internal("Close gamepad: " + gamepad.caps.Name);
            gamepad.Close();
        }
    }
}

GamepadCapabilities GamepadLinux::GetCapabilities(PlayerIndex playerIndex) const
{
    const auto index = GamepadHelper::ToInt(playerIndex);
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(gamepads.size()));
    return gamepads[index].caps;
}

GamepadState GamepadLinux::GetState(PlayerIndex playerIndex) const
{
    const auto index = GamepadHelper::ToInt(playerIndex);
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(gamepads.size()));
    return gamepads[index].state;
}

void GamepadLinux::EnumerateDevices()
{
    auto gamepad = std::begin(gamepads);

    for (int i = 0; i < 32; i++) {
        while ((gamepad != std::end(gamepads)) && gamepad->HasFileDescriptor()) {
            ++gamepad;
        }
        if (gamepad == std::end(gamepads)) {
            break;
        }

        auto iter = std::find_if(std::begin(gamepads), std::end(gamepads), [&](auto& dev) {
            return (dev.HasFileDescriptor() && (dev.deviceEventIndex == i));
        });
        if (iter != std::end(gamepads)) {
            // The device is already opened.
            continue;
        }

        POMDOG_ASSERT(!gamepad->HasFileDescriptor());

        if (!gamepad->Open(i)) {
            gamepad->Close();
            continue;
        }

        this->Connected(gamepad->playerIndex, gamepad->caps);
    }
}

void GamepadLinux::PollEvents()
{
    for (auto& gamepad : gamepads) {
        if (!gamepad.HasFileDescriptor()) {
            continue;
        }

        if (!gamepad.PollEvents()) {
            auto caps = gamepad.caps;
            gamepad.Close();
            this->Disconnected(gamepad.playerIndex, caps);
        }
    }
}

} // namespace Linux
} // namespace InputSystem
} // namespace Detail
} // namespace Pomdog
