// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Input/ButtonState.hpp"
#include "Pomdog/Input/KeyState.hpp"
#include "Pomdog/Input/Keys.hpp"
#include "Pomdog/Math/Point2D.hpp"
#include <string>

namespace Pomdog {
namespace Detail {

struct WindowWillCloseEvent {};
struct WindowShouldCloseEvent {};

struct ViewNeedsUpdateSurfaceEvent {};
struct ViewWillStartLiveResizeEvent {};
struct ViewDidEndLiveResizeEvent {};

struct InputTextEvent {
    std::string text;
};

struct InputKeyEvent {
    KeyState State;
    Keys Key;
};

enum class MouseEventType {
    Entered,
    Moved,
    Exited,
};

struct MousePositionEvent {
    Point2D Position;
    MouseEventType Type;
};

enum class MouseButtons {
    Left,
    Middle,
    Right,
    XButton1,
    XButton2,
};

enum class MouseButtonState {
    Down,
    Dragged,
    Up,
};

struct MouseButtonEvent {
    Point2D Position;
    MouseButtons Button;
    MouseButtonState State;
};

struct ScrollWheelEvent {
    double ScrollingDeltaY;
};

} // namespace Detail
} // namespace Pomdog
