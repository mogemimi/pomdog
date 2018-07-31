// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "../InputSystem/GamepadMappings.hpp"
#include "Pomdog/Input/Gamepad.hpp"
#include "Pomdog/Input/GamepadCapabilities.hpp"
#include "Pomdog/Input/GamepadState.hpp"
#include <IOKit/hid/IOHIDManager.h>
#include <array>
#include <cstdint>
#include <memory>

namespace Pomdog {
namespace Detail {
namespace InputSystem {
namespace Apple {

struct ThumbStickInfo final {
    std::int32_t Minimum = 0;
    std::int32_t Range = 0;
    std::int32_t InvertDirection = 1;
};

class GamepadDevice final {
public:
    GamepadCapabilities caps;
    GamepadState state;
    IOHIDDeviceRef device = nullptr;
    GamepadMappings mappings;
    std::array<ThumbStickInfo, 6> thumbStickInfos;

public:
    bool Open();

    void Close();

    void OnDeviceInput(IOReturn result, void* sender, IOHIDValueRef value);
};

class GamepadIOKit final : public Gamepad {
public:
    GamepadIOKit();

    ~GamepadIOKit();

    GamepadCapabilities GetCapabilities(PlayerIndex index) const override;

    GamepadState GetState(PlayerIndex index) const override;

private:
    std::array<GamepadDevice, 4> gamepads;
    IOHIDManagerRef hidManager;

    void OnDeviceAttached(IOReturn result, void* sender, IOHIDDeviceRef device);
    void OnDeviceDetached(IOReturn result, void* sender, IOHIDDeviceRef device);
};

} // namespace Apple
} // namespace InputSystem
} // namespace Detail
} // namespace Pomdog
