// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include "MouseWin32.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace Win32 {
//-----------------------------------------------------------------------
MouseWin32::MouseWin32(HWND windowHandleIn)
    : windowHandle(windowHandleIn)
{
    POMDOG_ASSERT(windowHandle != nullptr);

    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(windowHandle, &cursorPos);
    state.Position = Point2D{ cursorPos.x, cursorPos.y };
}
//-----------------------------------------------------------------------
void MouseWin32::HandleMessage(RAWMOUSE const& mouse)
{
    if (mouse.usFlags == MOUSE_MOVE_RELATIVE) {
        previousState.Position = state.Position;
        //state.Position += Point2D{ mouse.lLastX, mouse.lLastY };

        POINT cursorPos;
        GetCursorPos(&cursorPos);
        ScreenToClient(windowHandle, &cursorPos);
        state.Position = Point2D{ cursorPos.x, cursorPos.y };
    }
    else if (mouse.usFlags & MOUSE_MOVE_ABSOLUTE) {
        previousState.Position = state.Position;
        //state.Position = Point2D{ mouse.lLastX, mouse.lLastY };

        POINT cursorPos;
        GetCursorPos(&cursorPos);
        ScreenToClient(windowHandle, &cursorPos);
        state.Position = Point2D{ cursorPos.x, cursorPos.y };
    }

    if (mouse.usButtonFlags & RI_MOUSE_WHEEL) {
        previousState.ScrollWheel = state.ScrollWheel;
        state.ScrollWheel += *(SHORT*)(&mouse.usButtonData);
    }

    // Not implemented
    //if (mouse.usButtonFlags & MOUSE_HWHEEL) {
    //    // mouse horizontal wheel
    //}

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN) {
        previousState.LeftButton = state.LeftButton;
        state.LeftButton = ButtonState::Pressed;
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP) {
        previousState.LeftButton = state.LeftButton;
        state.LeftButton = ButtonState::Released;
    }

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN) {
        previousState.RightButton = state.RightButton;
        state.RightButton = ButtonState::Pressed;
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP) {
        previousState.RightButton = state.RightButton;
        state.RightButton = ButtonState::Released;
    }

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN) {
        previousState.MiddleButton = state.MiddleButton;
        state.MiddleButton = ButtonState::Pressed;
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP) {
        previousState.MiddleButton = state.MiddleButton;
        state.MiddleButton = ButtonState::Released;
    }

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) {
        previousState.XButton1 = state.XButton1;
        state.XButton1 = ButtonState::Pressed;
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP) {
        previousState.XButton1 = state.XButton1;
        state.XButton1 = ButtonState::Released;
    }

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) {
        previousState.XButton2 = state.XButton2;
        state.XButton2 = ButtonState::Pressed;
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP) {
        previousState.XButton2 = state.XButton2;
        state.XButton2 = ButtonState::Released;
    }
}
//-----------------------------------------------------------------------
MouseState MouseWin32::GetState() const
{
    return state;
}
//-----------------------------------------------------------------------
} // namespace Win32
} // namespace Detail
} // namespace Pomdog
