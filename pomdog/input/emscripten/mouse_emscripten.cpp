// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/emscripten/mouse_emscripten.h"
#include "pomdog/application/backends/system_event_queue.h"
#include "pomdog/application/backends/system_events.h"
#include "pomdog/input/backends/mouse_impl.h"
#include "pomdog/input/mouse_buttons.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::emscripten {
namespace {

[[nodiscard]] std::optional<MouseButtons>
translateMouseButton(unsigned short button) noexcept
{
    switch (button) {
    case 0:
        return MouseButtons::Left;
    case 1:
        return MouseButtons::Middle;
    case 2:
        return MouseButtons::Right;
    case 3:
        return MouseButtons::X1;
    case 4:
        return MouseButtons::X2;
    default:
        break;
    }
    return std::nullopt;
}

[[nodiscard]] EM_BOOL
handleMouseEvent(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) noexcept
{
    auto* eventQueue = reinterpret_cast<SystemEventQueue*>(userData);
    if (eventQueue == nullptr) {
        return EM_FALSE;
    }

    const auto position = Point2D{
        static_cast<int>(mouseEvent->targetX),
        static_cast<int>(mouseEvent->targetY),
    };

    switch (eventType) {
    case EMSCRIPTEN_EVENT_MOUSEDOWN: {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseMovedEvent,
            .data = MousePositionEvent{.position = position},
        });
        if (auto mb = translateMouseButton(mouseEvent->button); mb.has_value()) {
            eventQueue->enqueue(SystemEvent{
                .kind = SystemEventKind::MouseButtonEvent,
                .data = MouseButtonEvent{.button = *mb, .state = ButtonState::Down},
            });
        }
        return EM_TRUE;
    }
    case EMSCRIPTEN_EVENT_MOUSEUP: {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseMovedEvent,
            .data = MousePositionEvent{.position = position},
        });
        if (auto mb = translateMouseButton(mouseEvent->button); mb.has_value()) {
            eventQueue->enqueue(SystemEvent{
                .kind = SystemEventKind::MouseButtonEvent,
                .data = MouseButtonEvent{.button = *mb, .state = ButtonState::Up},
            });
        }
        return EM_TRUE;
    }
    case EMSCRIPTEN_EVENT_MOUSEMOVE: {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseMovedEvent,
            .data = MousePositionEvent{.position = position},
        });
        return EM_TRUE;
    }
    case EMSCRIPTEN_EVENT_MOUSEENTER: {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseEnteredEvent,
            .data = MousePositionEvent{.position = position},
        });
        return EM_TRUE;
    }
    case EMSCRIPTEN_EVENT_MOUSELEAVE: {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::MouseExitedEvent,
            .data = MousePositionEvent{.position = position},
        });
        return EM_TRUE;
    }
    default:
        break;
    }

    return EM_FALSE;
}

[[nodiscard]] EM_BOOL
handleWheelEvent(
    [[maybe_unused]] int eventType,
    const EmscriptenWheelEvent* wheelEvent,
    void* userData) noexcept
{
    auto* eventQueue = reinterpret_cast<SystemEventQueue*>(userData);
    if (eventQueue == nullptr) {
        return EM_FALSE;
    }

    // NOTE: Emscripten wheel deltaY is positive when scrolling down,
    // but our convention is positive = scroll up, so we negate both axes.
    // Normalize to "notch" units based on deltaMode.
    f64 deltaX = wheelEvent->deltaX;
    f64 deltaY = wheelEvent->deltaY;

    switch (wheelEvent->deltaMode) {
    case DOM_DELTA_PIXEL:
        // Typically ~100 pixels per notch on most browsers.
        deltaX /= 100.0;
        deltaY /= 100.0;
        break;
    case DOM_DELTA_LINE:
        // 3 lines per notch is a common default.
        deltaX /= 3.0;
        deltaY /= 3.0;
        break;
    case DOM_DELTA_PAGE:
        // 1 page is already large; treat as-is.
        break;
    default:
        break;
    }

    eventQueue->enqueue(SystemEvent{
        .kind = SystemEventKind::ScrollWheelEvent,
        .data = ScrollWheelEvent{
            .scrollingDeltaX = -deltaX,
            .scrollingDeltaY = -deltaY,
        },
    });
    return EM_TRUE;
}

} // namespace

MouseEmscripten::MouseEmscripten(const std::shared_ptr<MouseImpl>& impl) noexcept
    : impl_(impl)
{
    POMDOG_ASSERT(impl_ != nullptr);
}

MouseEmscripten::~MouseEmscripten() noexcept = default;

void MouseEmscripten::subscribeEvent(const std::string& targetCanvas, const std::shared_ptr<SystemEventQueue>& eventQueue) noexcept
{
    auto* eq = eventQueue.get();
    const auto target = targetCanvas.c_str();
    emscripten_set_mouseenter_callback(target, eq, EM_TRUE, handleMouseEvent);
    emscripten_set_mouseleave_callback(target, eq, EM_TRUE, handleMouseEvent);
    emscripten_set_mousemove_callback(target, eq, EM_TRUE, handleMouseEvent);
    emscripten_set_mousedown_callback(target, eq, EM_TRUE, handleMouseEvent);
    emscripten_set_mouseup_callback(target, eq, EM_TRUE, handleMouseEvent);
    emscripten_set_wheel_callback(target, eq, EM_TRUE, handleWheelEvent);
}

void MouseEmscripten::unsubscribeEvent(const std::string& targetCanvas) noexcept
{
    const auto target = targetCanvas.c_str();
    emscripten_set_mouseenter_callback(target, nullptr, EM_TRUE, nullptr);
    emscripten_set_mouseleave_callback(target, nullptr, EM_TRUE, nullptr);
    emscripten_set_mousemove_callback(target, nullptr, EM_TRUE, nullptr);
    emscripten_set_mousedown_callback(target, nullptr, EM_TRUE, nullptr);
    emscripten_set_mouseup_callback(target, nullptr, EM_TRUE, nullptr);
    emscripten_set_wheel_callback(target, nullptr, EM_TRUE, nullptr);
}

void MouseEmscripten::handleMessage(const SystemEvent& event)
{
    POMDOG_ASSERT(impl_ != nullptr);

    switch (event.kind) {
    case SystemEventKind::MouseEnteredEvent: {
        const auto ev = std::get<MousePositionEvent>(event.data);
        impl_->setPosition(ev.position);
        impl_->setPresent(true);
        break;
    }
    case SystemEventKind::MouseExitedEvent: {
        const auto ev = std::get<MousePositionEvent>(event.data);
        impl_->setPosition(ev.position);
        impl_->setPresent(false);
        break;
    }
    case SystemEventKind::MouseMovedEvent: {
        const auto ev = std::get<MousePositionEvent>(event.data);
        impl_->setPosition(ev.position);
        break;
    }
    case SystemEventKind::ScrollWheelEvent: {
        const auto ev = std::get<ScrollWheelEvent>(event.data);
        impl_->addScroll(ev.scrollingDeltaX, ev.scrollingDeltaY);
        break;
    }
    case SystemEventKind::MouseButtonEvent: {
        const auto ev = std::get<MouseButtonEvent>(event.data);
        impl_->setButton(ev.button, ev.state);
        break;
    }
    default:
        break;
    }
}

} // namespace pomdog::detail::emscripten
