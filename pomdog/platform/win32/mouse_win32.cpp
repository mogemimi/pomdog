// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/platform/win32/mouse_win32.hpp"
#include "pomdog/input/mouse_buttons.hpp"
#include "pomdog/signals/event_queue.hpp"
#include "pomdog/utility/assert.hpp"

namespace Pomdog::Detail::Win32 {

MouseWin32::MouseWin32(HWND windowHandleIn)
    : windowHandle(windowHandleIn)
{
    POMDOG_ASSERT(windowHandle != nullptr);

    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(windowHandle, &cursorPos);
    state.Position = Point2D{cursorPos.x, cursorPos.y};
}

void MouseWin32::HandleMessage(const SystemEvent& event)
{
    switch (event.Kind) {
    case SystemEventKind::MouseMovedEvent: {
        const auto ev = std::get<MousePositionEvent>(event.Data);
        previousState.Position = state.Position;
        state.Position = ev.Position;
        Mouse::Moved(state.Position);
        break;
    }
    case SystemEventKind::ScrollWheelEvent: {
        const auto ev = std::get<ScrollWheelWin32Event>(event.Data);
        previousState.ScrollWheel = state.ScrollWheel;
        state.ScrollWheel += ev.ScrollingDeltaY;
        Mouse::ScrollWheel(state.ScrollWheel - previousState.ScrollWheel);
        break;
    }
    case SystemEventKind::MouseButtonEvent: {
        const auto ev = std::get<MouseButtonWin32Event>(event.Data);
        switch (ev.Button) {
        case MouseButtons::Left:
            previousState.LeftButton = state.LeftButton;
            state.LeftButton = ev.State;
            break;
        case MouseButtons::Right:
            previousState.RightButton = state.RightButton;
            state.RightButton = ev.State;
            break;
        case MouseButtons::Middle:
            previousState.MiddleButton = state.MiddleButton;
            state.MiddleButton = ev.State;
            break;
        case MouseButtons::XButton1:
            previousState.XButton1 = state.XButton1;
            state.XButton1 = ev.State;
            break;
        case MouseButtons::XButton2:
            previousState.XButton2 = state.XButton2;
            state.XButton2 = ev.State;
            break;
        }

        switch (ev.State) {
        case ButtonState::Pressed:
            Mouse::ButtonDown(ev.Button);
            break;
        case ButtonState::Released:
            Mouse::ButtonUp(ev.Button);
            break;
        }
        break;
    }
    default:
        break;
    }
}

MouseState MouseWin32::GetState() const
{
    return state;
}

void TranslateMouseEvent(HWND windowHandle, const RAWMOUSE& mouse, const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue) noexcept
{
    if (mouse.usFlags == MOUSE_MOVE_RELATIVE) {
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        ScreenToClient(windowHandle, &cursorPos);
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::MouseMovedEvent,
            .Data = MousePositionEvent{
                .Position = Point2D{cursorPos.x, cursorPos.y},
            },
        });
    }
    else if (mouse.usFlags & MOUSE_MOVE_ABSOLUTE) {
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        ScreenToClient(windowHandle, &cursorPos);
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::MouseMovedEvent,
            .Data = MousePositionEvent{
                .Position = Point2D{cursorPos.x, cursorPos.y},
            },
        });
    }

    if (mouse.usButtonFlags & RI_MOUSE_WHEEL) {
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::ScrollWheelEvent,
            .Data = ScrollWheelWin32Event{
                .ScrollingDeltaY = *reinterpret_cast<const SHORT*>(&mouse.usButtonData),
            },
        });
    }

    // Not implemented
    //if (mouse.usButtonFlags & MOUSE_HWHEEL) {
    //    // mouse horizontal wheel
    //}

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN) {
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::MouseButtonEvent,
            .Data = MouseButtonWin32Event{
                .Button = MouseButtons::Left,
                .State = ButtonState::Pressed,
            },
        });
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP) {
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::MouseButtonEvent,
            .Data = MouseButtonWin32Event{
                .Button = MouseButtons::Left,
                .State = ButtonState::Released,
            },
        });
    }

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN) {
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::MouseButtonEvent,
            .Data = MouseButtonWin32Event{
                .Button = MouseButtons::Right,
                .State = ButtonState::Pressed,
            },
        });
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP) {
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::MouseButtonEvent,
            .Data = MouseButtonWin32Event{
                .Button = MouseButtons::Right,
                .State = ButtonState::Released,
            },
        });
    }

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN) {
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::MouseButtonEvent,
            .Data = MouseButtonWin32Event{
                .Button = MouseButtons::Middle,
                .State = ButtonState::Pressed,
            },
        });
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP) {
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::MouseButtonEvent,
            .Data = MouseButtonWin32Event{
                .Button = MouseButtons::Middle,
                .State = ButtonState::Released,
            },
        });
    }

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) {
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::MouseButtonEvent,
            .Data = MouseButtonWin32Event{
                .Button = MouseButtons::XButton1,
                .State = ButtonState::Pressed,
            },
        });
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP) {
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::MouseButtonEvent,
            .Data = MouseButtonWin32Event{
                .Button = MouseButtons::XButton1,
                .State = ButtonState::Released,
            },
        });
    }

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) {
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::MouseButtonEvent,
            .Data = MouseButtonWin32Event{
                .Button = MouseButtons::XButton2,
                .State = ButtonState::Pressed,
            },
        });
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP) {
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::MouseButtonEvent,
            .Data = MouseButtonWin32Event{
                .Button = MouseButtons::XButton2,
                .State = ButtonState::Released,
            },
        });
    }
}

} // namespace Pomdog::Detail::Win32
