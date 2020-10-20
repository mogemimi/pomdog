// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Platform.hpp"
#include "Pomdog/Input/ButtonState.hpp"
#include "Pomdog/Input/GamepadCapabilities.hpp"
#include "Pomdog/Input/KeyState.hpp"
#include "Pomdog/Input/Keys.hpp"
#include "Pomdog/Input/MouseButtons.hpp"
#include "Pomdog/Input/PlayerIndex.hpp"
#include "Pomdog/Math/Point2D.hpp"
#include <string>
#include <variant>

namespace Pomdog::Detail {

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

} // namespace Pomdog::Detail
