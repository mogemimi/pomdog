// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/system_events.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/backends/gamepad_mappings.h"
#include "pomdog/input/gamepad.h"
#include "pomdog/input/gamepad_capabilities.h"
#include "pomdog/input/gamepad_state.h"
#include "pomdog/signals/forward_declarations.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <IOKit/hid/IOHIDManager.h>
#include <array>
#include <cstdint>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

template <typename Event>
class EventQueue;

} // namespace pomdog

namespace pomdog::detail::IOKit {

struct ThumbStickInfo final {
    std::int32_t minimum = 0;
    std::int32_t range = 0;
    std::int32_t invertDirection = 1;
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
    void close();

    void onDeviceInput(IOReturn result, void* sender, IOHIDValueRef value);
};

class GamepadIOKit final : public Gamepad {
private:
    std::shared_ptr<EventQueue<SystemEvent>> eventQueue_;
    std::array<GamepadDevice, 4> gamepads_;
    IOHIDManagerRef hidManager_ = nullptr;

public:
    GamepadIOKit();

    ~GamepadIOKit() override;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue);

    [[nodiscard]] GamepadCapabilities
    getCapabilities(PlayerIndex index) const override;

    [[nodiscard]] GamepadState
    getState(PlayerIndex index) const override;

    void handleEvent(const SystemEvent& event);

private:
    void onDeviceAttached(IOReturn result, void* sender, IOHIDDeviceRef device);
    void onDeviceDetached(IOReturn result, void* sender, IOHIDDeviceRef device);
};

} // namespace pomdog::detail::IOKit
