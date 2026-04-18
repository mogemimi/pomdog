// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/emscripten/touchscreen_emscripten.h"
#include "pomdog/application/backends/system_event_queue.h"
#include "pomdog/application/backends/system_events.h"
#include "pomdog/input/mouse.h"
#include "pomdog/input/mouse_buttons.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::emscripten {
namespace {

[[nodiscard]] EM_BOOL
handleTouchEvent(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData) noexcept
{
    auto* eventQueue = reinterpret_cast<SystemEventQueue*>(userData);
    if (eventQueue == nullptr) {
        return EM_FALSE;
    }

    if (touchEvent->numTouches < 1) {
        return EM_FALSE;
    }

    const auto& touchPoint = touchEvent->touches[0];

    const auto position = Point2D{
        static_cast<int>(touchPoint.targetX),
        static_cast<int>(touchPoint.targetY),
    };

    switch (eventType) {
    case EMSCRIPTEN_EVENT_TOUCHSTART: {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::TouchscreenEvent,
            .data = detail::TouchscreenEvent{.position = position, .kind = TouchEventKind::TouchStart},
        });
        return EM_TRUE;
    }
    case EMSCRIPTEN_EVENT_TOUCHEND: {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::TouchscreenEvent,
            .data = detail::TouchscreenEvent{.position = position, .kind = TouchEventKind::TouchEnd},
        });
        return EM_TRUE;
    }
    case EMSCRIPTEN_EVENT_TOUCHMOVE: {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::TouchscreenEvent,
            .data = detail::TouchscreenEvent{.position = position, .kind = TouchEventKind::TouchMove},
        });
        return EM_TRUE;
    }
    case EMSCRIPTEN_EVENT_TOUCHCANCEL: {
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::TouchscreenEvent,
            .data = detail::TouchscreenEvent{.position = position, .kind = TouchEventKind::TouchCancel},
        });
        return EM_TRUE;
    }
    default:
        break;
    }

    return EM_FALSE;
}

} // namespace

TouchscreenEmscripten::TouchscreenEmscripten() noexcept
{
    std::fill(position_.begin(), position_.end(), Point2D{0, 0});
    std::fill(pressed_.begin(), pressed_.end(), false);
}

TouchscreenEmscripten::~TouchscreenEmscripten() noexcept = default;

bool TouchscreenEmscripten::isMouseSimulationEnabled() const noexcept
{
    return mouseSimulationEnabled_;
}

void TouchscreenEmscripten::setMouseSimulationEnabled(bool enabled) noexcept
{
    mouseSimulationEnabled_ = enabled;
}

u8 TouchscreenEmscripten::getSupportedTouchCount() const noexcept
{
    return static_cast<u8>(position_.size());
}

Point2D TouchscreenEmscripten::getPosition(u8 touchIndex) const noexcept
{
    if (touchIndex >= position_.size()) {
        return Point2D{0, 0};
    }
    return position_[touchIndex];
}

bool TouchscreenEmscripten::isTouchPressed(u8 touchIndex) const noexcept
{
    if (touchIndex >= pressed_.size()) {
        return false;
    }
    return pressed_[touchIndex];
}

Point2D TouchscreenEmscripten::getPrimaryTouchPosition() const noexcept
{
    return position_[0];
}

bool TouchscreenEmscripten::isPrimaryTouchPressed() const noexcept
{
    return pressed_[0];
}

void TouchscreenEmscripten::subscribeEvent(const std::string& targetCanvas, const std::shared_ptr<SystemEventQueue>& eventQueue) noexcept
{
    auto* eq = eventQueue.get();
    const auto target = targetCanvas.c_str();
    emscripten_set_touchstart_callback(target, eq, EM_TRUE, handleTouchEvent);
    emscripten_set_touchend_callback(target, eq, EM_TRUE, handleTouchEvent);
    emscripten_set_touchmove_callback(target, eq, EM_TRUE, handleTouchEvent);
    emscripten_set_touchcancel_callback(target, eq, EM_TRUE, handleTouchEvent);
}

void TouchscreenEmscripten::unsubscribeEvent(const std::string& targetCanvas) noexcept
{
    const auto target = targetCanvas.c_str();
    emscripten_set_touchstart_callback(target, nullptr, EM_TRUE, nullptr);
    emscripten_set_touchend_callback(target, nullptr, EM_TRUE, nullptr);
    emscripten_set_touchmove_callback(target, nullptr, EM_TRUE, nullptr);
    emscripten_set_touchcancel_callback(target, nullptr, EM_TRUE, nullptr);
}

void TouchscreenEmscripten::handleMessage(const SystemEvent& event)
{
    if (event.kind != SystemEventKind::TouchscreenEvent) {
        return;
    }

    const auto ev = std::get<detail::TouchscreenEvent>(event.data);
    position_[0] = ev.position;

    switch (ev.kind) {
    case TouchEventKind::TouchStart:
        pressed_[0] = true;
        break;
    case TouchEventKind::TouchMove:
        pressed_[0] = true;
        break;
    case TouchEventKind::TouchEnd:
        pressed_[0] = false;
        break;
    case TouchEventKind::TouchCancel:
        pressed_[0] = false;
        break;
    }
}

void TouchscreenEmscripten::simulateUsingMouse(const Mouse& mouse) noexcept
{
    if (mouse.isButtonDown(MouseButtons::Left)) {
        position_[0] = mouse.getPosition();
        pressed_[0] = true;
    }
    else {
        pressed_[0] = false;
    }
}

} // namespace pomdog::detail::emscripten
