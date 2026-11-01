// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/linux/gamepad_linux.h"
#include "pomdog/input/backends/gamepad_helper.h"
#include "pomdog/input/backends/gamepad_impl.h"
#include "pomdog/input/game_controller_db.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/diagnostic_queue.h"
#include "pomdog/utility/error_helper.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <array>
#include <charconv>
#include <cstring>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <tuple>
#include <utility>
#include <vector>
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

[[nodiscard]] std::string
makeDevicePath(int deviceIndex)
{
    return "/dev/input/event" + std::to_string(deviceIndex);
}

/// Returns the result of a failed call made while opening a device. A device
/// that was removed meanwhile is not a failure, since the next enumeration no
/// longer finds it.
[[nodiscard]] std::tuple<GamepadOpenResult, std::unique_ptr<Error>>
makeOpenFailure(int errnum, std::string&& message)
{
    if ((errnum == ENOENT) || (errnum == ENODEV) || (errnum == ENXIO)) {
        return std::make_tuple(GamepadOpenResult::NotGamepad, nullptr);
    }
    return std::make_tuple(GamepadOpenResult::NotGamepad, errors::fromErrc(errors::toErrc(errnum), std::move(message)));
}

/// Reads the lowest word of a capability bitmap in sysfs, which the kernel
/// prints as hexadecimal words separated by spaces, highest word first.
[[nodiscard]] std::optional<u64>
readLowestCapabilityWord(const std::string& path)
{
    const int file = ::open(path.c_str(), O_RDONLY | O_CLOEXEC);
    if (file < 0) {
        return std::nullopt;
    }

    // NOTE: The ev and abs bitmaps take at most two words, so a full buffer
    // means unexpected content.
    std::array<char, 64> buffer;
    const auto size = ::read(file, buffer.data(), buffer.size());
    ::close(file);
    if ((size <= 0) || (size >= static_cast<ssize_t>(buffer.size()))) {
        return std::nullopt;
    }

    std::string_view text(buffer.data(), static_cast<std::size_t>(size));
    while (!text.empty() && (text.back() == '\n')) {
        text.remove_suffix(1);
    }
    if (const auto space = text.rfind(' '); space != std::string_view::npos) {
        text.remove_prefix(space + 1);
    }

    u64 word = 0;
    const auto [end, ec] = std::from_chars(text.data(), text.data() + text.size(), word, 16);
    if ((ec != std::errc{}) || (end != text.data() + text.size())) {
        return std::nullopt;
    }
    return word;
}

[[nodiscard]] bool
hasBitInWord(u64 word, u32 bit) noexcept
{
    return ((word >> bit) & 1) != 0;
}

/// Reports whether sysfs describes /dev/input/event{deviceIndex} as a gamepad,
/// by the same test as GamepadDevice::open(). Unlike the device node, sysfs is
/// readable without access to the device.
[[nodiscard]] bool
isGamepadInSysfs(int deviceIndex)
{
    const auto dir = "/sys/class/input/event" + std::to_string(deviceIndex) + "/device/capabilities/";
    const auto evBits = readLowestCapabilityWord(dir + "ev");
    const auto absBits = readLowestCapabilityWord(dir + "abs");
    if (!evBits.has_value() || !absBits.has_value()) {
        return false;
    }
    return hasBitInWord(*evBits, EV_KEY) &&
           hasBitInWord(*evBits, EV_ABS) &&
           hasBitInWord(*absBits, ABS_X) &&
           hasBitInWord(*absBits, ABS_Y);
}

} // namespace

std::tuple<GamepadOpenResult, std::unique_ptr<Error>>
GamepadDevice::open(int deviceIndex, const GameControllerDB& gameControllerDB)
{
    POMDOG_ASSERT(fd == -1);

    const auto path = makeDevicePath(deviceIndex);
    fd = ::open(path.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        const auto errnum = errno;
        fd = -1;
        if (((errnum == EACCES) || (errnum == EPERM)) && !isGamepadInSysfs(deviceIndex)) {
            // NOTE: Without root or the input group, keyboards and mice
            // usually cannot be opened, while udev grants access to gamepads.
            // Only a gamepad that cannot be opened is a failure.
            return std::make_tuple(GamepadOpenResult::NotGamepad, nullptr);
        }
        return makeOpenFailure(errnum, "::open() failed: " + path);
    }

    POMDOG_ASSERT(fd >= 0);

    struct input_id inputId;
    if (::ioctl(fd, EVIOCGID, &inputId) < 0) {
        const auto errnum = errno;
        close();
        return makeOpenFailure(errnum, "::ioctl(EVIOCGID) failed: " + path);
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
        const auto errnum = errno;
        close();
        return makeOpenFailure(errnum, "::ioctl(EVIOCGBIT) failed: " + path);
    }

    if (!hasBit(evBits, EV_KEY) ||
        !hasBit(evBits, EV_ABS) ||
        !hasBit(absBits, ABS_X) ||
        !hasBit(absBits, ABS_Y)) {
        // The device is not a joystick or gamepad.
        close();
        return std::make_tuple(GamepadOpenResult::NotGamepad, nullptr);
    }

    std::tie(this->mappings, this->caps.name) = gameControllerDB.getMappings(uuid);

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
                const auto errnum = errno;
                close();
                return makeOpenFailure(errnum, "::ioctl(EVIOCGABS) failed: " + path);
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
            const auto errnum = errno;
            close();
            return makeOpenFailure(errnum, "::ioctl(EVIOCGNAME) failed: " + path);
        }
        caps.name = gamepadName.data();
    }

    // NOTE: gamepad is open.
    POMDOG_ASSERT(impl != nullptr);
    impl->connect();
    impl->setName(caps.name);
    impl->setCapabilities(caps);

    this->deviceEventIndex = deviceIndex;

    return std::make_tuple(GamepadOpenResult::Opened, nullptr);
}

void GamepadDevice::close()
{
    if (fd >= 0) {
        ::close(fd);
        fd = -1;
    }

    if (impl) {
        impl->disconnect();
    }

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
            const bool isDown = (event.value != 0);
            gamepad_mappings::applyButton(*impl, mappings.buttons, buttonIndex, isDown);
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
                        // horizontal: release left and right
                        gamepad_mappings::applyButton(*impl, mappings.hats[3], false);
                        gamepad_mappings::applyButton(*impl, mappings.hats[1], false);
                    }
                    else {
                        // vertical: release up and down
                        gamepad_mappings::applyButton(*impl, mappings.hats[0], false);
                        gamepad_mappings::applyButton(*impl, mappings.hats[2], false);
                    }
                }
                else if (event.value > 0) {
                    if (axis == 0) {
                        gamepad_mappings::applyButton(*impl, mappings.hats[1], true);
                    }
                    else {
                        gamepad_mappings::applyButton(*impl, mappings.hats[2], true);
                    }
                }
                else if (event.value < 0) {
                    if (axis == 0) {
                        gamepad_mappings::applyButton(*impl, mappings.hats[3], true);
                    }
                    else {
                        gamepad_mappings::applyButton(*impl, mappings.hats[0], true);
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
                const auto value = normalizeAxisValue(event.value, thumbStickInfos[event.code]);

                gamepad_mappings::applyThumbStick(*impl, mapper.thumbStick, value);

                if (mapper.positiveTrigger != ButtonKind::Invalid) {
                    constexpr float threshold = 0.05f;
                    gamepad_mappings::applyButton(*impl, mapper.positiveTrigger, (value > threshold));
                }

                if (mapper.negativeTrigger != ButtonKind::Invalid) {
                    constexpr float threshold = -0.05f;
                    gamepad_mappings::applyButton(*impl, mapper.negativeTrigger, (value < threshold));
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

GamepadServiceLinux::GamepadServiceLinux() noexcept = default;

std::unique_ptr<Error>
GamepadServiceLinux::initialize(
    std::shared_ptr<const GameControllerDB> gameControllerDB,
    std::shared_ptr<DiagnosticQueue> diagnostics) noexcept
{
    if (diagnostics == nullptr) {
        return errors::make("diagnostics must be != nullptr");
    }
    diagnostics_ = std::move(diagnostics);

    for (int i = 0; i < 4; ++i) {
        gamepads_[i].playerIndex = static_cast<PlayerIndex>(i + 1);
        gamepads_[i].impl = std::make_shared<GamepadImpl>();
        gamepads_[i].impl->setPlayerIndex(gamepads_[i].playerIndex);
    }

    gameControllerDB_ = std::move(gameControllerDB);
    if (gameControllerDB_ == nullptr) {
        gameControllerDB_ = createGameControllerDBDummy();
    }
    return nullptr;
}

GamepadServiceLinux::~GamepadServiceLinux()
{
    for (auto& gamepad : gamepads_) {
        if (gamepad.hasFileDescriptor()) {
            gamepad.close();
        }
    }
}

std::shared_ptr<Gamepad>
GamepadServiceLinux::getGamepad(PlayerIndex playerIndex) noexcept
{
    const auto index = GamepadHelper::toInt(playerIndex);
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(gamepads_.size()));
    return gamepads_[index].impl;
}

void GamepadServiceLinux::enumerateDevices()
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

        const auto path = makeDevicePath(i);
        struct stat status;
        if (::stat(path.c_str(), &status) != 0) {
            // NOTE: No device has this index now. Forget a device skipped
            // here, so that a device plugged in later is tried.
            std::erase_if(skippedDevices_, [i](const SkippedDevice& skipped) {
                return skipped.deviceIndex == i;
            });
            continue;
        }
        const SkippedDevice currentDevice{
            .deviceIndex = i,
            .inode = static_cast<u64>(status.st_ino),
            .changeTimeSeconds = static_cast<i64>(status.st_ctim.tv_sec),
            .changeTimeNanoseconds = static_cast<i64>(status.st_ctim.tv_nsec),
        };

        auto skipped = std::find_if(std::begin(skippedDevices_), std::end(skippedDevices_), [i](const SkippedDevice& device) {
            return device.deviceIndex == i;
        });
        if (skipped != std::end(skippedDevices_)) {
            if (*skipped == currentDevice) {
                continue;
            }
            // NOTE: Retry after a reconnect or a permission/ACL update on the
            // existing node, including access granted after hotplug detection.
            skippedDevices_.erase(skipped);
        }

        POMDOG_ASSERT(!gamepad->hasFileDescriptor());

        auto [openResult, openErr] = gamepad->open(i, *gameControllerDB_);
        if (openErr != nullptr) {
            skippedDevices_.push_back(currentDevice);
            diagnostics_->enqueue(errors::wrap(
                std::move(openErr),
                "failed to open a game controller; skipping it until it is reconnected or its permissions change"));
            continue;
        }
        if (openResult == GamepadOpenResult::NotGamepad) {
            skippedDevices_.push_back(currentDevice);
            continue;
        }
    }
}

void GamepadServiceLinux::pollEvents()
{
    for (auto& gamepad : gamepads_) {
        if (!gamepad.hasFileDescriptor()) {
            continue;
        }

        if (!gamepad.pollEvents()) {
            gamepad.close();
        }
    }
}

} // namespace pomdog::detail::linux
