// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/win32/mouse_win32.h"
#include "pomdog/application/system_events.h"
#include "pomdog/input/backends/mouse_impl.h"
#include "pomdog/input/mouse_buttons.h"
#include "pomdog/signals/event_queue.h"
#include "pomdog/utility/assert.h"

namespace pomdog::detail::win32 {

MouseWin32::MouseWin32(HWND windowHandle, const std::shared_ptr<MouseImpl>& impl)
    : impl_(impl)
{
    POMDOG_ASSERT(windowHandle != nullptr);
    POMDOG_ASSERT(impl_ != nullptr);

    POINT cursorPos;
    ::GetCursorPos(&cursorPos);
    ::ScreenToClient(windowHandle, &cursorPos);
    impl_->setPosition(Point2D{cursorPos.x, cursorPos.y});
}

void MouseWin32::handleMessage(const SystemEvent& event)
{
    POMDOG_ASSERT(impl_ != nullptr);

    switch (event.kind) {
    case SystemEventKind::MouseMovedEvent: {
        const auto ev = std::get<MousePositionEvent>(event.data);
        impl_->setPosition(ev.position);
        break;
    }
    case SystemEventKind::ScrollWheelEvent: {
        const auto ev = std::get<ScrollWheelWin32Event>(event.data);
        impl_->addScroll(ev.scrollingDeltaX, ev.scrollingDeltaY);
        break;
    }
    case SystemEventKind::MouseButtonEvent: {
        const auto ev = std::get<MouseButtonWin32Event>(event.data);
        impl_->setButton(ev.button, ev.state);
        break;
    }
    default:
        break;
    }
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
        constexpr f64 wheelDelta = static_cast<f64>(WHEEL_DELTA);
        const f64 delta = static_cast<f64>(static_cast<SHORT>(mouse.usButtonData)) / wheelDelta;
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::ScrollWheelEvent,
            .data = ScrollWheelWin32Event{
                .scrollingDeltaX = 0.0,
                .scrollingDeltaY = delta,
            },
        });
    }

    if (mouse.usButtonFlags & RI_MOUSE_HWHEEL) {
        constexpr f64 wheelDelta = static_cast<f64>(WHEEL_DELTA);
        const f64 delta = static_cast<f64>(static_cast<SHORT>(mouse.usButtonData)) / wheelDelta;
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::ScrollWheelEvent,
            .data = ScrollWheelWin32Event{
                .scrollingDeltaX = delta,
                .scrollingDeltaY = 0.0,
            },
        });
    }

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
