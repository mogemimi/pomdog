// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "MouseX11.hpp"
#include "Pomdog/Input/MouseButtons.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <optional>

namespace Pomdog {
namespace Detail {
namespace X11 {
namespace {

ButtonState* GetButtonByIndex(MouseState & mouseState, unsigned int buttonIndex)
{
    switch (buttonIndex) {
    case 1: return &mouseState.LeftButton;
    case 2: return &mouseState.MiddleButton;
    case 3: return &mouseState.RightButton;
    default:
        break;
    }
    return nullptr;
}

std::optional<MouseButtons> ToMouseButtons(unsigned int buttonIndex)
{
    switch (buttonIndex) {
    case 1: return MouseButtons::Left;
    case 2: return MouseButtons::Middle;
    case 3: return MouseButtons::Right;
    default:
        break;
    }
    return std::nullopt;
}

} // unnamed namespace

MouseState MouseX11::GetState() const
{
    return mouseState;
}

void MouseX11::HandleEvent(XEvent & event)
{
    switch (event.type) {
    case ButtonPress: {
        if (auto button = GetButtonByIndex(mouseState, event.xbutton.button)) {
            *button = ButtonState::Pressed;

            auto mouseButton = ToMouseButtons(event.xbutton.button);
            POMDOG_ASSERT(mouseButton);
            Mouse::ButtonDown(*mouseButton);
        }
//        else if (event.xbutton.button == 4) {
//            mouseState.ScrollWheel += 1;
//        }
//        else if (event.xbutton.button == 5) {
//            mouseState.ScrollWheel -= 1;
//        }
        break;
    }
    case ButtonRelease: {
        if (auto button = GetButtonByIndex(mouseState, event.xbutton.button)) {
            *button = ButtonState::Released;

            auto mouseButton = ToMouseButtons(event.xbutton.button);
            POMDOG_ASSERT(mouseButton);
            Mouse::ButtonUp(*mouseButton);
        }
//        else if (event.xbutton.button == 4) {
//            mouseState.ScrollWheel += 1;
//        }
//        else if (event.xbutton.button == 5) {
//            mouseState.ScrollWheel -= 1;
//        }
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

} // namespace X11
} // namespace Detail
} // namespace Pomdog
