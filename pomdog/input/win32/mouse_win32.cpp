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
        state_.position = ev.position;
        break;
    }
    case SystemEventKind::ScrollWheelEvent: {
        const auto ev = std::get<ScrollWheelWin32Event>(event.data);
        state_.scrollWheel += ev.scrollingDeltaY;
        break;
    }
    case SystemEventKind::MouseButtonEvent: {
        const auto ev = std::get<MouseButtonWin32Event>(event.data);
        switch (ev.button) {
        case MouseButtons::Left:
            state_.leftButton = ev.state;
            break;
        case MouseButtons::Right:
            state_.rightButton = ev.state;
            break;
        case MouseButtons::Middle:
            state_.middleButton = ev.state;
            break;
        case MouseButtons::X1:
            state_.xButton1 = ev.state;
            break;
        case MouseButtons::X2:
            state_.xButton2 = ev.state;
            break;
        }
        break;
    }
    default:
        break;
    }
}

Point2D MouseWin32::getPosition() const noexcept
{
    return state_.position;
}

bool MouseWin32::isButtonDown(MouseButtons button) const noexcept
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

i32 MouseWin32::getScrollX() const noexcept
{
    return 0;
}

i32 MouseWin32::getScrollY() const noexcept
{
    return state_.scrollWheel;
}

bool MouseWin32::isPresent() const noexcept
{
    return true;
}

void MouseWin32::clearAllButtons() noexcept
{
    state_.leftButton = ButtonState::Up;
    state_.middleButton = ButtonState::Up;
    state_.rightButton = ButtonState::Up;
    state_.xButton1 = ButtonState::Up;
    state_.xButton2 = ButtonState::Up;
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
                .scrollingDeltaY = static_cast<SHORT>(mouse.usButtonData),
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
                .state = ButtonState::Down,
            },
        });
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP) {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseButtonEvent,
            .data = MouseButtonWin32Event{
                .button = MouseButtons::Left,
                .state = ButtonState::Up,
            },
        });
    }

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN) {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseButtonEvent,
            .data = MouseButtonWin32Event{
                .button = MouseButtons::Right,
                .state = ButtonState::Down,
            },
        });
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP) {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseButtonEvent,
            .data = MouseButtonWin32Event{
                .button = MouseButtons::Right,
                .state = ButtonState::Up,
            },
        });
    }

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN) {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseButtonEvent,
            .data = MouseButtonWin32Event{
                .button = MouseButtons::Middle,
                .state = ButtonState::Down,
            },
        });
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP) {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseButtonEvent,
            .data = MouseButtonWin32Event{
                .button = MouseButtons::Middle,
                .state = ButtonState::Up,
            },
        });
    }

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseButtonEvent,
            .data = MouseButtonWin32Event{
                .button = MouseButtons::X1,
                .state = ButtonState::Down,
            },
        });
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP) {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseButtonEvent,
            .data = MouseButtonWin32Event{
                .button = MouseButtons::X1,
                .state = ButtonState::Up,
            },
        });
    }

    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseButtonEvent,
            .data = MouseButtonWin32Event{
                .button = MouseButtons::X2,
                .state = ButtonState::Down,
            },
        });
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP) {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseButtonEvent,
            .data = MouseButtonWin32Event{
                .button = MouseButtons::X2,
                .state = ButtonState::Up,
            },
        });
    }
}

} // namespace pomdog::detail::win32
