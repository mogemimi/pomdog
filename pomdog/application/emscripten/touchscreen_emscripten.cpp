// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/emscripten/touchscreen_emscripten.h"
#include "pomdog/application/backends/system_event_queue.h"
#include "pomdog/application/backends/system_events.h"
#include "pomdog/input/mouse.h"
#include "pomdog/input/mouse_buttons.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <algorithm>
#include <iterator>
#include <variant>
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

    TouchEventKind kind = {};
    switch (eventType) {
    case EMSCRIPTEN_EVENT_TOUCHSTART:
        kind = TouchEventKind::TouchStart;
        break;
    case EMSCRIPTEN_EVENT_TOUCHMOVE:
        kind = TouchEventKind::TouchMove;
        break;
    case EMSCRIPTEN_EVENT_TOUCHEND:
        kind = TouchEventKind::TouchEnd;
        break;
    case EMSCRIPTEN_EVENT_TOUCHCANCEL:
        kind = TouchEventKind::TouchCancel;
        break;
    default:
        return EM_FALSE;
    }

    // NOTE: touches lists every touch point on the screen, and marks those
    // that this event is about.
    const auto count = std::clamp(touchEvent->numTouches, 0, static_cast<int>(std::size(touchEvent->touches)));
    for (int i = 0; i < count; ++i) {
        const auto& touchPoint = touchEvent->touches[i];
        if (!touchPoint.isChanged) {
            continue;
        }
        eventQueue->enqueue(SystemEvent{
            .kind = SystemEventKind::TouchscreenEvent,
            .data = detail::TouchscreenEvent{
                .position = Point2D{touchPoint.targetX, touchPoint.targetY},
                .identifier = touchPoint.identifier,
                .kind = kind,
            },
        });
    }

    // NOTE: Taking the events keeps the browser from scrolling or zooming the
    // page, and from sending the mouse events that it emulates for touches.
    return EM_TRUE;
}

} // namespace

TouchscreenEmscripten::TouchscreenEmscripten() noexcept = default;

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

void TouchscreenEmscripten::beginFrame() noexcept
{
    for (std::size_t i = 0; i < maxTouchCount; ++i) {
        if (releasePending_[i]) {
            pressed_[i] = false;
        }
    }
    startedInFrame_.fill(false);
    releasePending_.fill(false);
}

void TouchscreenEmscripten::handleMessage(const SystemEvent& event)
{
    if (event.kind != SystemEventKind::TouchscreenEvent) {
        return;
    }

    const auto ev = std::get<detail::TouchscreenEvent>(event.data);
    if (ev.kind == TouchEventKind::TouchStart) {
        // NOTE: Touches beyond the supported count are ignored until a slot
        // frees up. Skip slots whose release waits for the next frame.
        for (std::size_t i = 0; i < maxTouchCount; ++i) {
            if (!pressed_[i]) {
                position_[i] = ev.position;
                identifiers_[i] = ev.identifier;
                pressed_[i] = true;
                startedInFrame_[i] = true;
                break;
            }
        }
        return;
    }

    for (std::size_t i = 0; i < maxTouchCount; ++i) {
        if (!pressed_[i] || releasePending_[i] || (identifiers_[i] != ev.identifier)) {
            continue;
        }
        position_[i] = ev.position;
        if (ev.kind == TouchEventKind::TouchMove) {
            break;
        }
        if (startedInFrame_[i]) {
            releasePending_[i] = true;
        }
        else {
            pressed_[i] = false;
        }
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
