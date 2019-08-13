// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "MouseWin32.hpp"
#include "Pomdog/Input/MouseButtons.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog::Detail::Win32 {

MouseWin32::MouseWin32(HWND windowHandleIn)
    : windowHandle(windowHandleIn)
{
    POMDOG_ASSERT(windowHandle != nullptr);

    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(windowHandle, &cursorPos);
    state.Position = Point2D{ cursorPos.x, cursorPos.y };
}

void MouseWin32::HandleMessage(const RAWMOUSE& mouse)
{
    if (mouse.usFlags == MOUSE_MOVE_RELATIVE) {
        previousState.Position = state.Position;
        //state.Position += Point2D{ mouse.lLastX, mouse.lLastY };

        POINT cursorPos;
        GetCursorPos(&cursorPos);
        ScreenToClient(windowHandle, &cursorPos);
        state.Position = Point2D{ cursorPos.x, cursorPos.y };
        Mouse::Moved(state.Position);
    }
    else if (mouse.usFlags & MOUSE_MOVE_ABSOLUTE) {
        previousState.Position = state.Position;
        //state.Position = Point2D{ mouse.lLastX, mouse.lLastY };

        POINT cursorPos;
        GetCursorPos(&cursorPos);
        ScreenToClient(windowHandle, &cursorPos);
        state.Position = Point2D{ cursorPos.x, cursorPos.y };
        Mouse::Moved(state.Position);
    }

    if (mouse.usButtonFlags & RI_MOUSE_WHEEL) {
        previousState.ScrollWheel = state.ScrollWheel;
        state.ScrollWheel += *(SHORT*)(&mouse.usButtonData);
        Mouse::ScrollWheel(state.ScrollWheel - previousState.ScrollWheel);
    }

    // Not implemented
    //if (mouse.usButtonFlags & MOUSE_HWHEEL) {
    //    // mouse horizontal wheel
    //}

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN) {
        previousState.LeftButton = state.LeftButton;
        state.LeftButton = ButtonState::Pressed;
        Mouse::ButtonDown(MouseButtons::Left);
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP) {
        previousState.LeftButton = state.LeftButton;
        state.LeftButton = ButtonState::Released;
        Mouse::ButtonUp(MouseButtons::Left);
    }

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN) {
        previousState.RightButton = state.RightButton;
        state.RightButton = ButtonState::Pressed;
        Mouse::ButtonDown(MouseButtons::Right);
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP) {
        previousState.RightButton = state.RightButton;
        state.RightButton = ButtonState::Released;
        Mouse::ButtonUp(MouseButtons::Right);
    }

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN) {
        previousState.MiddleButton = state.MiddleButton;
        state.MiddleButton = ButtonState::Pressed;
        Mouse::ButtonDown(MouseButtons::Middle);
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP) {
        previousState.MiddleButton = state.MiddleButton;
        state.MiddleButton = ButtonState::Released;
        Mouse::ButtonUp(MouseButtons::Middle);
    }

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) {
        previousState.XButton1 = state.XButton1;
        state.XButton1 = ButtonState::Pressed;
        Mouse::ButtonDown(MouseButtons::XButton1);
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP) {
        previousState.XButton1 = state.XButton1;
        state.XButton1 = ButtonState::Released;
        Mouse::ButtonUp(MouseButtons::XButton1);
    }

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) {
        previousState.XButton2 = state.XButton2;
        state.XButton2 = ButtonState::Pressed;
        Mouse::ButtonDown(MouseButtons::XButton2);
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP) {
        previousState.XButton2 = state.XButton2;
        state.XButton2 = ButtonState::Released;
        Mouse::ButtonUp(MouseButtons::XButton2);
    }
}

MouseState MouseWin32::GetState() const
{
    return state;
}

} // namespace Pomdog::Detail::Win32
