// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/backends/mouse_impl.h"
#include "pomdog/input/button_state.h"
#include "pomdog/input/mouse_buttons.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/enum_cast.h"

namespace pomdog::detail {

MouseImpl::MouseImpl() = default;

Point2D MouseImpl::getPosition() const noexcept
{
    return position_;
}

bool MouseImpl::isButtonDown(MouseButtons button) const noexcept
{
    POMDOG_ASSERT(to_underlying(button) < buttons_.size());
    return buttons_.test(to_underlying(button));
}

f64 MouseImpl::getScrollX() const noexcept
{
    return scrollX_;
}

f64 MouseImpl::getScrollY() const noexcept
{
    return scrollY_;
}

f64 MouseImpl::getScrollDeltaX() const noexcept
{
    return scrollDeltaX_;
}

f64 MouseImpl::getScrollDeltaY() const noexcept
{
    return scrollDeltaY_;
}

bool MouseImpl::isPresent() const noexcept
{
    return present_;
}

void MouseImpl::setPosition(const Point2D& position) noexcept
{
    position_ = position;
}

void MouseImpl::setButton(MouseButtons button, ButtonState state) noexcept
{
    POMDOG_ASSERT(to_underlying(button) < buttons_.size());
    buttons_.set(to_underlying(button), (state == ButtonState::Down));
}

void MouseImpl::addScroll(f64 deltaX, f64 deltaY) noexcept
{
    scrollX_ += deltaX;
    scrollY_ += deltaY;
    scrollDeltaX_ += deltaX;
    scrollDeltaY_ += deltaY;
}

void MouseImpl::setPresent(bool present) noexcept
{
    present_ = present;
}

void MouseImpl::clearAllButtons() noexcept
{
    buttons_.reset();
}

void MouseImpl::clearScrollDelta() noexcept
{
    scrollDeltaX_ = 0.0;
    scrollDeltaY_ = 0.0;
}

} // namespace pomdog::detail
