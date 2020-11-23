// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "../Application/SystemEvents.hpp"
#include "../Input.Backends/GamepadMappings.hpp"
#include "Pomdog/Input/Gamepad.hpp"
#include "Pomdog/Input/GamepadCapabilities.hpp"
#include "Pomdog/Input/GamepadState.hpp"
#include "Pomdog/Signals/ForwardDeclarations.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <IOKit/hid/IOHIDManager.h>
#include <array>
#include <cstdint>
#include <memory>

namespace Pomdog {

template <typename Event>
class EventQueue;

} // namespace Pomdog

namespace Pomdog::Detail::IOKit {

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
    PlayerIndex playerIndex;

public:
    void Close();

    void OnDeviceInput(IOReturn result, void* sender, IOHIDValueRef value);
};

class GamepadIOKit final : public Gamepad {
public:
    GamepadIOKit();

    ~GamepadIOKit() override;

    [[nodiscard]] std::shared_ptr<Error>
    Initialize(const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue);

    GamepadCapabilities GetCapabilities(PlayerIndex index) const override;

    GamepadState GetState(PlayerIndex index) const override;

    void HandleEvent(const SystemEvent& event);

private:
    std::shared_ptr<EventQueue<SystemEvent>> eventQueue;
    std::array<GamepadDevice, 4> gamepads;
    IOHIDManagerRef hidManager = nullptr;

    void OnDeviceAttached(IOReturn result, void* sender, IOHIDDeviceRef device);
    void OnDeviceDetached(IOReturn result, void* sender, IOHIDDeviceRef device);
};

} // namespace Pomdog::Detail::IOKit
