// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/x11/mouse_x11.h"
#include "pomdog/input/mouse_buttons.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::x11 {
namespace {

[[nodiscard]] ButtonState*
getButtonByIndex(MouseState& mouseState, unsigned int buttonIndex) noexcept
{
    switch (buttonIndex) {
    case Button1:
        return &mouseState.leftButton;
    case Button2:
        return &mouseState.middleButton;
    case Button3:
        return &mouseState.rightButton;
    default:
        break;
    }
    return nullptr;
}

} // namespace

void MouseX11::handleEvent(XEvent& event)
{
    switch (event.type) {
    case ButtonPress: {
        if (auto button = getButtonByIndex(mouseState_, event.xbutton.button); button != nullptr) {
            *button = ButtonState::Down;
        }
        else if (event.xbutton.button == Button4) {
            mouseState_.scrollWheel += 1;
        }
        else if (event.xbutton.button == Button5) {
            mouseState_.scrollWheel -= 1;
        }
        break;
    }
    case ButtonRelease: {
        if (auto button = getButtonByIndex(mouseState_, event.xbutton.button); button != nullptr) {
            *button = ButtonState::Up;
        }
        break;
    }
    case EnterNotify: {
        break;
    }
    case MotionNotify: {
        mouseState_.position.x = event.xmotion.x;
        mouseState_.position.y = event.xmotion.y;
        break;
    }
    case LeaveNotify: {
        break;
    }
    }
}

Point2D MouseX11::getPosition() const noexcept
{
    return mouseState_.position;
}

bool MouseX11::isButtonDown(MouseButtons button) const noexcept
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

i32 MouseX11::getScrollX() const noexcept
{
    return 0;
}

i32 MouseX11::getScrollY() const noexcept
{
    return mouseState_.scrollWheel;
}

bool MouseX11::isPresent() const noexcept
{
    return true;
}

void MouseX11::clearAllButtons() noexcept
{
    mouseState_.leftButton = ButtonState::Up;
    mouseState_.middleButton = ButtonState::Up;
    mouseState_.rightButton = ButtonState::Up;
    mouseState_.xButton1 = ButtonState::Up;
    mouseState_.xButton2 = ButtonState::Up;
}

} // namespace pomdog::detail::x11
