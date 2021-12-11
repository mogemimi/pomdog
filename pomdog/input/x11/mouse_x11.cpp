// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/x11/mouse_x11.hpp"
#include "pomdog/input/mouse_buttons.hpp"
#include "pomdog/utility/assert.hpp"
#include <optional>

namespace pomdog::detail::x11 {
namespace {

ButtonState* GetButtonByIndex(MouseState& mouseState, unsigned int buttonIndex)
{
    switch (buttonIndex) {
    case Button1:
        return &mouseState.LeftButton;
    case Button2:
        return &mouseState.MiddleButton;
    case Button3:
        return &mouseState.RightButton;
    default:
        break;
    }
    return nullptr;
}

std::optional<MouseButtons> ToMouseButtons(unsigned int buttonIndex)
{
    switch (buttonIndex) {
    case Button1:
        return MouseButtons::Left;
    case Button2:
        return MouseButtons::Middle;
    case Button3:
        return MouseButtons::Right;
    default:
        break;
    }
    return std::nullopt;
}

} // namespace

MouseState MouseX11::GetState() const
{
    return mouseState;
}

void MouseX11::HandleEvent(XEvent& event)
{
    switch (event.type) {
    case ButtonPress: {
        if (auto button = GetButtonByIndex(mouseState, event.xbutton.button); button != nullptr) {
            *button = ButtonState::Pressed;

            auto mouseButton = ToMouseButtons(event.xbutton.button);
            POMDOG_ASSERT(mouseButton);
            Mouse::ButtonDown(*mouseButton);
        }
        else if (event.xbutton.button == Button4) {
            const auto previousScrollWheel = mouseState.ScrollWheel;
            mouseState.ScrollWheel += 1;
            Mouse::ScrollWheel(mouseState.ScrollWheel - previousScrollWheel);
        }
        else if (event.xbutton.button == Button5) {
            const auto previousScrollWheel = mouseState.ScrollWheel;
            mouseState.ScrollWheel -= 1;
            Mouse::ScrollWheel(mouseState.ScrollWheel - previousScrollWheel);
        }
        break;
    }
    case ButtonRelease: {
        if (auto button = GetButtonByIndex(mouseState, event.xbutton.button); button != nullptr) {
            *button = ButtonState::Released;

            auto mouseButton = ToMouseButtons(event.xbutton.button);
            POMDOG_ASSERT(mouseButton);
            Mouse::ButtonUp(*mouseButton);
        }
        break;
    }
    case EnterNotify: {
        break;
    }
    case MotionNotify: {
        mouseState.Position.X = event.xmotion.x;
        mouseState.Position.Y = event.xmotion.y;
        Mouse::Moved(mouseState.Position);
        break;
    }
    case LeaveNotify: {
        break;
    }
    }
}

} // namespace pomdog::detail::x11
