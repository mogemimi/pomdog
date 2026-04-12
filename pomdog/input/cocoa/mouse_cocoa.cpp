// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/cocoa/mouse_cocoa.h"
#include "pomdog/application/system_events.h"
#include "pomdog/input/backends/mouse_impl.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::cocoa {
namespace {

[[nodiscard]] ButtonState
toButtonState(MouseButtonState mouseButtonState) noexcept
{
    switch (mouseButtonState) {
    case MouseButtonState::Up:
        return ButtonState::Up;
    case MouseButtonState::Down:
        return ButtonState::Down;
    case MouseButtonState::Dragged:
        return ButtonState::Down;
    }
}

} // namespace

MouseCocoa::MouseCocoa(const std::shared_ptr<MouseImpl>& impl)
    : impl_(impl)
{
    POMDOG_ASSERT(impl_ != nullptr);
}

void MouseCocoa::handleEvent(const SystemEvent& event)
{
    POMDOG_ASSERT(impl_ != nullptr);

    switch (event.kind) {
    case SystemEventKind::MouseEnteredEvent:
        [[fallthrough]];
    case SystemEventKind::MouseMovedEvent:
        [[fallthrough]];
    case SystemEventKind::MouseExitedEvent: {
        const auto ev = std::get<MousePositionEvent>(event.data);
        static_assert(sizeof(ev) <= 24);
        impl_->setPosition(ev.position);
        break;
    }
    case SystemEventKind::MouseButtonEvent: {
        const auto ev = std::get<MouseButtonCocoaEvent>(event.data);
        static_assert(sizeof(ev) <= 24);
        impl_->setButton(ev.button, toButtonState(ev.state));
        impl_->setPosition(ev.position);
        break;
    }
    case SystemEventKind::ScrollWheelEvent: {
        const auto ev = std::get<ScrollWheelCocoaEvent>(event.data);
        static_assert(sizeof(ev) <= 24);
        scrollWheel_ += ev.scrollingDeltaY;
        static_assert(std::is_same_v<f64, decltype(scrollWheel_)>);
        impl_->setScrollWheel(static_cast<std::int32_t>(scrollWheel_));
        break;
    }
    default:
        break;
    }
}

} // namespace pomdog::detail::cocoa
