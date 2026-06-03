// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/win32/mouse_win32.h"
#include "pomdog/application/backends/system_event_queue.h"
#include "pomdog/application/backends/system_events.h"
#include "pomdog/input/backends/mouse_impl.h"
#include "pomdog/input/mouse_buttons.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::win32 {
namespace {

[[nodiscard]] Point2D
clientToLogical(HWND windowHandle, POINT physical) noexcept
{
    // NOTE: In Per-Monitor V2 DPI awareness mode the client-area coordinates
    // returned by ScreenToClient are in physical pixels. Convert to logical
    // pixels (matching the OS coordinate system) by dividing by the window's
    // raw DPI scale. Mouse position uses the OS raw DPR rather than the
    // engine's effective pixel ratio so it stays consistent with the on-screen
    // cursor regardless of `HighDPISettings::maxPixelRatio`.
    const UINT dpi = ::GetDpiForWindow(windowHandle);
    const f32 dpr = (dpi > 0) ? (static_cast<f32>(dpi) / 96.0f) : 1.0f;
    if (dpr <= 1.0f) {
        return Point2D{physical.x, physical.y};
    }
    const f32 inv = 1.0f / dpr;
    return Point2D{
        static_cast<i32>(std::lround(static_cast<f32>(physical.x) * inv)),
        static_cast<i32>(std::lround(static_cast<f32>(physical.y) * inv)),
    };
}

} // namespace

MouseWin32::MouseWin32(HWND windowHandle, const std::shared_ptr<MouseImpl>& impl)
    : impl_(impl)
{
    POMDOG_ASSERT(windowHandle != nullptr);
    POMDOG_ASSERT(impl_ != nullptr);

    POINT cursorPos;
    ::GetCursorPos(&cursorPos);
    ::ScreenToClient(windowHandle, &cursorPos);
    impl_->setPosition(clientToLogical(windowHandle, cursorPos));
}

void MouseWin32::handleMessage(const SystemEvent& event)
{
    POMDOG_ASSERT(impl_ != nullptr);

    switch (event.kind) {
    case SystemEventKind::MouseEnteredEvent: {
        impl_->setPresent(true);
        break;
    }
    case SystemEventKind::MouseExitedEvent: {
        impl_->setPresent(false);
        break;
    }
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

void translateMouseEvent(HWND windowHandle, const RAWMOUSE& mouse, const std::shared_ptr<SystemEventQueue>& eventQueue) noexcept
{
    if (mouse.usFlags == MOUSE_MOVE_RELATIVE) {
        POINT cursorPos;
        ::GetCursorPos(&cursorPos);
        ::ScreenToClient(windowHandle, &cursorPos);
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseMovedEvent,
            .data = MousePositionEvent{
                .position = clientToLogical(windowHandle, cursorPos),
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
                .position = clientToLogical(windowHandle, cursorPos),
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
