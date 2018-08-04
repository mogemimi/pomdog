// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "MouseCocoa.hpp"
#include "../Application/SystemEvents.hpp"
#include "Pomdog/Signals/Event.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <type_traits>

namespace Pomdog {
namespace Detail {
namespace Cocoa {
namespace {

ButtonState ToButtonState(MouseButtonState mouseButtonState)
{
    switch (mouseButtonState) {
    case MouseButtonState::Up:
        return ButtonState::Released;
    case MouseButtonState::Down:
        return ButtonState::Pressed;
    case MouseButtonState::Dragged:
        return ButtonState::Pressed;
    }
}

} // unnamed namespace

MouseCocoa::MouseCocoa()
    : scrollWheel(0)
{
}

MouseState MouseCocoa::GetState() const
{
    return state;
}

void MouseCocoa::HandleEvent(const Event& event)
{
    if (auto mousePositionEvent = event.As<MousePositionEvent>()) {
        state.Position = mousePositionEvent->Position;
        Mouse::Moved(state.Position);
    }
    else if (auto mouseButtonEvent = event.As<MouseButtonEvent>()) {
        const auto buttonState = ToButtonState(mouseButtonEvent->State);

        switch (mouseButtonEvent->Button) {
        case MouseButtons::Left:
            state.LeftButton = buttonState;
            break;
        case MouseButtons::Right:
            state.RightButton = buttonState;
            break;
        case MouseButtons::Middle:
            state.MiddleButton = buttonState;
            break;
        case MouseButtons::XButton1:
            state.XButton1 = buttonState;
            break;
        case MouseButtons::XButton2:
            state.XButton2 = buttonState;
            break;
        }

        if (state.Position != mouseButtonEvent->Position) {
            state.Position = mouseButtonEvent->Position;
            Mouse::Moved(state.Position);
        }

        switch (mouseButtonEvent->State) {
        case MouseButtonState::Up:
            Mouse::ButtonUp(mouseButtonEvent->Button);
            break;
        case MouseButtonState::Down:
            Mouse::ButtonDown(mouseButtonEvent->Button);
            break;
        case MouseButtonState::Dragged:
            break;
        }
    }
    else if (auto scrollWheelEvent = event.As<ScrollWheelEvent>()) {
        auto wheelDelta = scrollWheelEvent->ScrollingDeltaY;
        scrollWheel += wheelDelta;
        static_assert(std::is_same<double, decltype(scrollWheel)>::value, "");
        static_assert(std::is_same<std::int32_t, decltype(state.ScrollWheel)>::value, "");

        auto oldScrollWheel = state.ScrollWheel;
        state.ScrollWheel = static_cast<std::int32_t>(this->scrollWheel);
        Mouse::ScrollWheel(state.ScrollWheel - oldScrollWheel);
    }
}

} // namespace Cocoa
} // namespace Detail
} // namespace Pomdog
