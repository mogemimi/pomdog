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
        return ButtonState::Up;
    case MouseButtonState::Down:
        return ButtonState::Down;
    case MouseButtonState::Dragged:
        return ButtonState::Down;
    }
}

} // namespace

MouseCocoa::MouseCocoa() = default;

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
        case MouseButtons::X1:
            state_.xButton1 = buttonState;
            break;
        case MouseButtons::X2:
            state_.xButton2 = buttonState;
            break;
        }

        if (state_.position != ev.position) {
            state_.position = ev.position;
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

        state_.scrollWheel = static_cast<std::int32_t>(scrollWheel_);
        break;
    }
    default:
        break;
    }
}

Point2D MouseCocoa::getPosition() const noexcept
{
    return state_.position;
}

bool MouseCocoa::isButtonDown(MouseButtons button) const noexcept
{
    switch (button) {
    case MouseButtons::Left:
        return state_.leftButton == ButtonState::Down;
    case MouseButtons::Right:
        return state_.rightButton == ButtonState::Down;
    case MouseButtons::Middle:
        return state_.middleButton == ButtonState::Down;
    case MouseButtons::X1:
        return state_.xButton1 == ButtonState::Down;
    case MouseButtons::X2:
        return state_.xButton2 == ButtonState::Down;
    }
    return false;
}

i32 MouseCocoa::getScrollX() const noexcept
{
    return 0;
}

i32 MouseCocoa::getScrollY() const noexcept
{
    return state_.scrollWheel;
}

bool MouseCocoa::isPresent() const noexcept
{
    return true;
}

void MouseCocoa::clearAllButtons() noexcept
{
    state_.leftButton = ButtonState::Up;
    state_.middleButton = ButtonState::Up;
    state_.rightButton = ButtonState::Up;
    state_.xButton1 = ButtonState::Up;
    state_.xButton2 = ButtonState::Up;
}

} // namespace pomdog::detail::cocoa
