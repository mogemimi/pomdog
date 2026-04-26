// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/window_mode.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/basic/types.h"
#include "pomdog/input/button_state.h"
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

enum class SystemEventKind : u8 {
    WindowWillCloseEvent,
    WindowShouldCloseEvent,
    WindowModeChangedEvent,
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
    TouchscreenEvent,
};

struct InputTextEvent final {
    std::string text;
};

struct InputKeyEvent final {
    KeyState state = {};
    Keys key = {};
};

struct MousePositionEvent final {
    Point2D position = {};
};

enum class MouseButtonState : u8 {
    Down,
    Dragged,
    Up,
};

struct MouseButtonEvent final {
    MouseButtons button = {};
    ButtonState state = {};
};

struct ScrollWheelEvent final {
    f64 scrollingDeltaX = {};
    f64 scrollingDeltaY = {};
};

struct WindowModeChangedEvent final {
    WindowMode windowMode = WindowMode::Windowed;
};

#if defined(POMDOG_PLATFORM_WIN32)
using MouseButtonWin32Event = MouseButtonEvent;
using ScrollWheelWin32Event = ScrollWheelEvent;
#endif
#if defined(POMDOG_PLATFORM_MACOSX)
struct MouseButtonCocoaEvent final {
    Point2D position = {};
    MouseButtons button = {};
    MouseButtonState state = {};
};

struct ScrollWheelCocoaEvent final {
    f64 scrollingDeltaX = {};
    f64 scrollingDeltaY = {};
};
#endif

enum class TouchEventKind : u8 {
    TouchStart,
    TouchMove,
    TouchEnd,
    TouchCancel,
};

struct TouchscreenEvent final {
    Point2D position = {};
    TouchEventKind kind = {};
};

class SystemEvent final {
public:
    using EventBody = std::variant<
        InputTextEvent,
        InputKeyEvent,
        MousePositionEvent,
        MouseButtonState,
        MouseButtonEvent,
        ScrollWheelEvent,
        WindowModeChangedEvent,
#if defined(POMDOG_PLATFORM_MACOSX)
        MouseButtonCocoaEvent,
        ScrollWheelCocoaEvent,
#endif
        TouchscreenEvent>;

    SystemEventKind kind = {};
    EventBody data = {};
};

} // namespace pomdog::detail
