// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/win32/mouse_win32.h"
#include "pomdog/input/mouse_buttons.h"
#include "pomdog/signals/event_queue.h"
#include "pomdog/utility/assert.h"

namespace pomdog::detail::win32 {

MouseWin32::MouseWin32(HWND windowHandleIn)
    : windowHandle_(windowHandleIn)
{
    POMDOG_ASSERT(windowHandle_ != nullptr);

    POINT cursorPos;
    ::GetCursorPos(&cursorPos);
    ::ScreenToClient(windowHandle_, &cursorPos);
    state_.position = Point2D{cursorPos.x, cursorPos.y};
}

void MouseWin32::handleMessage(const SystemEvent& event)
{
    switch (event.kind) {
    case SystemEventKind::MouseMovedEvent: {
        const auto ev = std::get<MousePositionEvent>(event.data);
        previousState_.position = state_.position;
        state_.position = ev.position;
        Mouse::Moved(state_.position);
        break;
    }
    case SystemEventKind::ScrollWheelEvent: {
        const auto ev = std::get<ScrollWheelWin32Event>(event.data);
        previousState_.scrollWheel = state_.scrollWheel;
        state_.scrollWheel += ev.scrollingDeltaY;
        Mouse::ScrollWheel(state_.scrollWheel - previousState_.scrollWheel);
        break;
    }
    case SystemEventKind::MouseButtonEvent: {
        const auto ev = std::get<MouseButtonWin32Event>(event.data);
        switch (ev.button) {
        case MouseButtons::Left:
            previousState_.leftButton = state_.leftButton;
            state_.leftButton = ev.state;
            break;
        case MouseButtons::Right:
            previousState_.rightButton = state_.rightButton;
            state_.rightButton = ev.state;
            break;
        case MouseButtons::Middle:
            previousState_.middleButton = state_.middleButton;
            state_.middleButton = ev.state;
            break;
        case MouseButtons::XButton1:
            previousState_.xButton1 = state_.xButton1;
            state_.xButton1 = ev.state;
            break;
        case MouseButtons::XButton2:
            previousState_.xButton2 = state_.xButton2;
            state_.xButton2 = ev.state;
            break;
        }

        switch (ev.state) {
        case ButtonState::Pressed:
            Mouse::ButtonDown(ev.button);
            break;
        case ButtonState::Released:
            Mouse::ButtonUp(ev.button);
            break;
        }
        break;
    }
    default:
        break;
    }
}

MouseState
MouseWin32::getState() const
{
    return state_;
}

void translateMouseEvent(HWND windowHandle, const RAWMOUSE& mouse, const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue) noexcept
{
    if (mouse.usFlags == MOUSE_MOVE_RELATIVE) {
        POINT cursorPos;
        ::GetCursorPos(&cursorPos);
        ::ScreenToClient(windowHandle, &cursorPos);
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseMovedEvent,
            .data = MousePositionEvent{
                .position = Point2D{cursorPos.x, cursorPos.y},
            },
        });
    }
    else if (mouse.usFlags & MOUSE_MOVE_ABSOLUTE) {
        POINT cursorPos;
        ::GetCursorPos(&cursorPos);
        ::ScreenToClient(windowHandle, &cursorPos);
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseMovedEvent,
            .data = MousePositionEvent{
                .position = Point2D{cursorPos.x, cursorPos.y},
            },
        });
    }

    if (mouse.usButtonFlags & RI_MOUSE_WHEEL) {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::ScrollWheelEvent,
            .data = ScrollWheelWin32Event{
                .scrollingDeltaY = *reinterpret_cast<const SHORT*>(&mouse.usButtonData),
            },
        });
    }

    // Not implemented
    //if (mouse.usButtonFlags & MOUSE_HWHEEL) {
    //    // mouse horizontal wheel
    //}

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN) {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseButtonEvent,
            .data = MouseButtonWin32Event{
                .button = MouseButtons::Left,
                .state = ButtonState::Pressed,
            },
        });
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP) {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseButtonEvent,
            .data = MouseButtonWin32Event{
                .button = MouseButtons::Left,
                .state = ButtonState::Released,
            },
        });
    }

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN) {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseButtonEvent,
            .data = MouseButtonWin32Event{
                .button = MouseButtons::Right,
                .state = ButtonState::Pressed,
            },
        });
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP) {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseButtonEvent,
            .data = MouseButtonWin32Event{
                .button = MouseButtons::Right,
                .state = ButtonState::Released,
            },
        });
    }

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN) {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseButtonEvent,
            .data = MouseButtonWin32Event{
                .button = MouseButtons::Middle,
                .state = ButtonState::Pressed,
            },
        });
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP) {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseButtonEvent,
            .data = MouseButtonWin32Event{
                .button = MouseButtons::Middle,
                .state = ButtonState::Released,
            },
        });
    }

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseButtonEvent,
            .data = MouseButtonWin32Event{
                .button = MouseButtons::XButton1,
                .state = ButtonState::Pressed,
            },
        });
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP) {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseButtonEvent,
            .data = MouseButtonWin32Event{
                .button = MouseButtons::XButton1,
                .state = ButtonState::Released,
            },
        });
    }

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseButtonEvent,
            .data = MouseButtonWin32Event{
                .button = MouseButtons::XButton2,
                .state = ButtonState::Pressed,
            },
        });
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP) {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseButtonEvent,
            .data = MouseButtonWin32Event{
                .button = MouseButtons::XButton2,
                .state = ButtonState::Released,
            },
        });
    }
}

} // namespace pomdog::detail::win32
