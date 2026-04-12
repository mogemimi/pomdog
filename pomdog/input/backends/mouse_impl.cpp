// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/backends/mouse_impl.h"
#include "pomdog/input/button_state.h"
#include "pomdog/input/mouse_buttons.h"

namespace pomdog::detail {

MouseImpl::MouseImpl() = default;

Point2D MouseImpl::getPosition() const noexcept
{
    return mouseState_.position;
}

bool MouseImpl::isButtonDown(MouseButtons button) const noexcept
{
    switch (button) {
    case MouseButtons::Left:
        return mouseState_.leftButton == ButtonState::Down;
    case MouseButtons::Right:
        return mouseState_.rightButton == ButtonState::Down;
    case MouseButtons::Middle:
        return mouseState_.middleButton == ButtonState::Down;
    case MouseButtons::X1:
        return mouseState_.xButton1 == ButtonState::Down;
    case MouseButtons::X2:
        return mouseState_.xButton2 == ButtonState::Down;
    }
    return false;
}

i32 MouseImpl::getScrollX() const noexcept
{
    return 0;
}

i32 MouseImpl::getScrollY() const noexcept
{
    return mouseState_.scrollWheel;
}

bool MouseImpl::isPresent() const noexcept
{
    return true;
}

void MouseImpl::setPosition(const Point2D& position) noexcept
{
    mouseState_.position = position;
}

void MouseImpl::setButton(MouseButtons button, ButtonState state) noexcept
{
    switch (button) {
    case MouseButtons::Left:
        mouseState_.leftButton = state;
        break;
    case MouseButtons::Right:
        mouseState_.rightButton = state;
        break;
    case MouseButtons::Middle:
        mouseState_.middleButton = state;
        break;
    case MouseButtons::X1:
        mouseState_.xButton1 = state;
        break;
    case MouseButtons::X2:
        mouseState_.xButton2 = state;
        break;
    }
}

void MouseImpl::setScrollWheel(i32 value) noexcept
{
    mouseState_.scrollWheel = value;
}

void MouseImpl::addScrollWheel(i32 delta) noexcept
{
    mouseState_.scrollWheel += delta;
}

void MouseImpl::clearAllButtons() noexcept
{
    mouseState_.leftButton = ButtonState::Up;
    mouseState_.middleButton = ButtonState::Up;
    mouseState_.rightButton = ButtonState::Up;
    mouseState_.xButton1 = ButtonState::Up;
    mouseState_.xButton2 = ButtonState::Up;
}

} // namespace pomdog::detail
