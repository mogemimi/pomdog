// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/cocoa/mouse_cocoa.h"
#include "pomdog/application/system_events.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::cocoa {
namespace {

[[nodiscard]] ButtonState
toButtonState(MouseButtonState mouseButtonState) noexcept
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

MouseCocoa::MouseCocoa() = default;

MouseState MouseCocoa::getState() const
{
    return state_;
}

void MouseCocoa::handleEvent(const SystemEvent& event)
{
    switch (event.kind) {
    case SystemEventKind::MouseEnteredEvent:
        [[fallthrough]];
    case SystemEventKind::MouseMovedEvent:
        [[fallthrough]];
    case SystemEventKind::MouseExitedEvent: {
        const auto ev = std::get<MousePositionEvent>(event.data);
        static_assert(sizeof(ev) <= 24);
        state_.position = ev.position;
        Mouse::Moved(state_.position);
        break;
    }
    case SystemEventKind::MouseButtonEvent: {
        const auto ev = std::get<MouseButtonCocoaEvent>(event.data);
        static_assert(sizeof(ev) <= 24);
        const auto buttonState = toButtonState(ev.state);

        switch (ev.button) {
        case MouseButtons::Left:
            state_.leftButton = buttonState;
            break;
        case MouseButtons::Right:
            state_.rightButton = buttonState;
            break;
        case MouseButtons::Middle:
            state_.middleButton = buttonState;
            break;
        case MouseButtons::XButton1:
            state_.xButton1 = buttonState;
            break;
        case MouseButtons::XButton2:
            state_.xButton2 = buttonState;
            break;
        }

        if (state_.position != ev.position) {
            state_.position = ev.position;
            Mouse::Moved(state_.position);
        }

        switch (ev.state) {
        case MouseButtonState::Up:
            Mouse::ButtonUp(ev.button);
            break;
        case MouseButtonState::Down:
            Mouse::ButtonDown(ev.button);
            break;
        case MouseButtonState::Dragged:
            break;
        }
        break;
    }
    case SystemEventKind::ScrollWheelEvent: {
        const auto ev = std::get<ScrollWheelCocoaEvent>(event.data);
        static_assert(sizeof(ev) <= 24);
        const auto wheelDelta = ev.scrollingDeltaY;
        scrollWheel_ += wheelDelta;
        static_assert(std::is_same<double, decltype(scrollWheel_)>::value, "");
        static_assert(std::is_same<std::int32_t, decltype(state_.scrollWheel)>::value, "");

        const auto oldScrollWheel = state_.scrollWheel;
        state_.scrollWheel = static_cast<std::int32_t>(scrollWheel_);
        Mouse::ScrollWheel(state_.scrollWheel - oldScrollWheel);
        break;
    }
    default:
        break;
    }
}

} // namespace pomdog::detail::cocoa
