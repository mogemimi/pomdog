// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include "MouseX11.hpp"

namespace Pomdog {
namespace Detail {
namespace X11 {
namespace {

static ButtonState* ToMouseButton(MouseState & mouseState, unsigned int buttonIndex)
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

} // unnamed namespace
//-----------------------------------------------------------------------
MouseState MouseX11::GetState() const
{
    return mouseState;
}
//-----------------------------------------------------------------------
void MouseX11::HandleEvent(XEvent & event)
{
    switch (event.type) {
    case ButtonPress: {
        if (auto button = ToMouseButton(mouseState, event.xbutton.button)) {
            *button = ButtonState::Pressed;
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
        if (auto button = ToMouseButton(mouseState, event.xbutton.button)) {
            *button = ButtonState::Released;
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
        break;
    }
    case LeaveNotify: {
        break;
    }
    }
}
//-----------------------------------------------------------------------
} // namespace X11
} // namespace Detail
} // namespace Pomdog
