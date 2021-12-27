// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/input/button_state.h"
#include "pomdog/input/gamepad_capabilities.h"
#include "pomdog/input/key_state.h"
#include "pomdog/input/keys.h"
#include "pomdog/input/mouse_buttons.h"
#include "pomdog/input/player_index.h"
#include "pomdog/math/point2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
#include <variant>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

enum class SystemEventKind : std::uint8_t {
    WindowWillCloseEvent,
    WindowShouldCloseEvent,
    ViewNeedsUpdateSurfaceEvent,
    ViewWillStartLiveResizeEvent,
    ViewDidEndLiveResizeEvent,
    InputTextEvent,
    InputKeyEvent,
    MouseEnteredEvent,
    MouseMovedEvent,
    MouseExitedEvent,
    MouseButtonEvent,
    ScrollWheelEvent,
    GamepadConnectedEvent,
    GamepadDisconnectedEvent,
};

struct InputTextEvent final {
    std::string Text;
};

struct InputKeyEvent final {
    KeyState State;
    Keys Key;
};

struct MousePositionEvent final {
    Point2D Position;
};

enum class MouseButtonState : std::uint8_t {
    Down,
    Dragged,
    Up,
};

#if defined(POMDOG_PLATFORM_WIN32)
struct MouseButtonWin32Event final {
    MouseButtons Button;
    ButtonState State;
};

struct ScrollWheelWin32Event final {
    std::int32_t ScrollingDeltaY;
};
#endif
#if defined(POMDOG_PLATFORM_MACOSX)
struct MouseButtonCocoaEvent final {
    Point2D Position;
    MouseButtons Button;
    MouseButtonState State;
};

struct ScrollWheelCocoaEvent final {
    double ScrollingDeltaY;
};
#endif

struct GamepadEvent final {
    PlayerIndex PlayerIndex;
    GamepadCapabilities Capabilities;
};

class SystemEvent final {
public:
    using EventBody = std::variant<
        InputTextEvent,
        InputKeyEvent,
        MousePositionEvent,
        MouseButtonState,
#if defined(POMDOG_PLATFORM_WIN32)
        MouseButtonWin32Event,
        ScrollWheelWin32Event,
#endif
#if defined(POMDOG_PLATFORM_MACOSX)
        MouseButtonCocoaEvent,
        ScrollWheelCocoaEvent,
#endif
        GamepadEvent>;

    SystemEventKind Kind;
    EventBody Data;
};

} // namespace pomdog::detail
