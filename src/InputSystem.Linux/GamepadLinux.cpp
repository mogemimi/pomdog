// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "GamepadLinux.hpp"
#include "../InputSystem/GamepadHelper.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <linux/input.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
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

ButtonState GetTriggerButtonValue(int value)
{
    return (value > 0) ? ButtonState::Pressed : ButtonState::Released;
}

float GetThumbStickValue(int value, const ThumbStickInfo& info)
{
    return (static_cast<float>((value - info.Minimum) * 2 - info.Range) / info.Range);
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

    GamepadDeviceID uuid;
    uuid.BusType = static_cast<uint16_t>(inputId.bustype);
    uuid.Vendor = static_cast<uint16_t>(inputId.vendor);
    uuid.Product = static_cast<uint16_t>(inputId.product);
    uuid.Version = static_cast<uint16_t>(inputId.version);

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

    int numOfButtons = 0;
    std::fill(std::begin(keyMap), std::end(keyMap), -1);

    for (int i = BTN_JOYSTICK; i < KEY_MAX; ++i) {
        if (HasBit(keyBits, static_cast<uint32_t>(i))) {
            const auto index = i - BTN_GAMEPAD;
            if ((index < 0) || (index >= static_cast<int>(keyMap.size()))) {
                continue;
            }
            keyMap[index] = static_cast<int8_t>(numOfButtons);
            ++numOfButtons;
        }
    }

    int numOfAxis = 0;
    for (int i = ABS_X; i <= ABS_RZ; ++i) {
        if (HasBit(absBits, i)) {
            struct input_absinfo absInfo;
            if (::ioctl(fd, EVIOCGABS(i), &absInfo) < 0) {
                continue;
            }

            POMDOG_ASSERT(i >= 0);
            POMDOG_ASSERT(i < static_cast<int>(thumbStickInfos.size()));
            thumbStickInfos[i].Minimum = absInfo.minimum;
            thumbStickInfos[i].Range = std::max(1, absInfo.maximum - absInfo.minimum);
            ++numOfAxis;
        }
    }

    std::array<char, 256> gamepadName;
    std::fill(std::begin(gamepadName), std::end(gamepadName), 0);
    if (::ioctl(fd, EVIOCGNAME(255), gamepadName.data()) < 0) {
        Close();
        return false;
    }

    caps.Name = gamepadName.data();
    caps.ThumbStickCount = std::max(0, numOfAxis);
    caps.ButtonCount = std::max(0, numOfButtons);

    std::string controllerName;
    std::tie(this->mappings, controllerName) = GetMappings(uuid);
    if (!controllerName.empty()) {
        this->caps.Name = controllerName;
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

    caps.Name = "";
    caps.ThumbStickCount = 0;
    caps.ButtonCount = 0;
}

bool GamepadDevice::HasFileDescriptor() const
{
    return fd >= 0;
}

void GamepadDevice::PollEvents()
{
    for (;;) {
        struct input_event event;

        errno = 0;
        if (::read(fd, &event, sizeof(event)) < 0) {
            if (errno == ENODEV) {
                Log::Internal("Disconnect gamepad: " + caps.Name);
                Close();
            }
            break;
        }

        switch (event.type) {
        case EV_KEY: {
            const auto physicalIndex = static_cast<int>(event.code) - BTN_GAMEPAD;
            if (physicalIndex >= static_cast<int>(keyMap.size())) {
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
            case ABS_RZ:
                static_assert(ABS_X == 0, "");
                static_assert(ABS_Y == 1, "");
                static_assert(ABS_Z == 2, "");
                static_assert(ABS_RX == 3, "");
                static_assert(ABS_RY == 4, "");
                static_assert(ABS_RZ == 5, "");
                POMDOG_ASSERT(event.code >= 0);
                POMDOG_ASSERT(event.code < static_cast<int>(mappings.axes.size()));
                POMDOG_ASSERT(event.code < static_cast<int>(thumbStickInfos.size()));

                switch (mappings.axes[event.code]) {
                case AxesKind::LeftTrigger:
                    state.Buttons.LeftTrigger = GetTriggerButtonValue(event.value);
                    break;
                case AxesKind::RightTrigger:
                    state.Buttons.RightTrigger = GetTriggerButtonValue(event.value);
                    break;
                case AxesKind::LeftStickX:
                    state.ThumbSticks.Left.X = GetThumbStickValue(event.value, thumbStickInfos[event.code]);
                    break;
                case AxesKind::LeftStickY:
                    state.ThumbSticks.Left.Y = GetThumbStickValue(event.value, thumbStickInfos[event.code]);
                    break;
                case AxesKind::RightStickX:
                    state.ThumbSticks.Right.X = GetThumbStickValue(event.value, thumbStickInfos[event.code]);
                    break;
                case AxesKind::RightStickY:
                    state.ThumbSticks.Right.Y = GetThumbStickValue(event.value, thumbStickInfos[event.code]);
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
}

GamepadLinux::GamepadLinux()
{
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
    POMDOG_ASSERT(index < static_cast<int>(gamepads.size()));
    return gamepads[index].caps;
}

GamepadState GamepadLinux::GetState(PlayerIndex playerIndex) const
{
    const auto index = GamepadHelper::ToInt(playerIndex);
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
    }
}

void GamepadLinux::PollEvents()
{
    EnumerateDevices();

    for (auto& gamepad : gamepads) {
        if (!gamepad.HasFileDescriptor()) {
            continue;
        }

        gamepad.PollEvents();
    }
}

} // namespace Linux
} // namespace InputSystem
} // namespace Detail
} // namespace Pomdog
