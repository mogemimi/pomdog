// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/system_events.hpp"
#include "pomdog/input/backends/gamepad_mappings.hpp"
#include "pomdog/input/gamepad.hpp"
#include "pomdog/input/gamepad_capabilities.hpp"
#include "pomdog/input/gamepad_state.hpp"
#include "pomdog/signals/forward_declarations.hpp"
#include "pomdog/utility/errors.hpp"
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

    [[nodiscard]] std::unique_ptr<Error>
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
