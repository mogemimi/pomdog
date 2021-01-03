// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "MouseCocoa.hpp"
#include "../Application/SystemEvents.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <type_traits>

namespace Pomdog::Detail::Cocoa {
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

} // namespace

MouseCocoa::MouseCocoa()
    : scrollWheel(0)
{
}

MouseState MouseCocoa::GetState() const
{
    return state;
}

void MouseCocoa::HandleEvent(const SystemEvent& event)
{
    switch (event.Kind) {
    case SystemEventKind::MouseEnteredEvent:
        [[fallthrough]];
    case SystemEventKind::MouseMovedEvent:
        [[fallthrough]];
    case SystemEventKind::MouseExitedEvent: {
        const auto ev = std::get<MousePositionEvent>(event.Data);
        static_assert(sizeof(ev) <= 24);
        state.Position = ev.Position;
        Mouse::Moved(state.Position);
        break;
    }
    case SystemEventKind::MouseButtonEvent: {
        const auto ev = std::get<MouseButtonCocoaEvent>(event.Data);
        static_assert(sizeof(ev) <= 24);
        const auto buttonState = ToButtonState(ev.State);

        switch (ev.Button) {
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

        if (state.Position != ev.Position) {
            state.Position = ev.Position;
            Mouse::Moved(state.Position);
        }

        switch (ev.State) {
        case MouseButtonState::Up:
            Mouse::ButtonUp(ev.Button);
            break;
        case MouseButtonState::Down:
            Mouse::ButtonDown(ev.Button);
            break;
        case MouseButtonState::Dragged:
            break;
        }
        break;
    }
    case SystemEventKind::ScrollWheelEvent: {
        const auto ev = std::get<ScrollWheelCocoaEvent>(event.Data);
        static_assert(sizeof(ev) <= 24);
        auto wheelDelta = ev.ScrollingDeltaY;
        scrollWheel += wheelDelta;
        static_assert(std::is_same<double, decltype(scrollWheel)>::value, "");
        static_assert(std::is_same<std::int32_t, decltype(state.ScrollWheel)>::value, "");

        auto oldScrollWheel = state.ScrollWheel;
        state.ScrollWheel = static_cast<std::int32_t>(this->scrollWheel);
        Mouse::ScrollWheel(state.ScrollWheel - oldScrollWheel);
        break;
    }
    default:
        break;
    }
}

} // namespace Pomdog::Detail::Cocoa
