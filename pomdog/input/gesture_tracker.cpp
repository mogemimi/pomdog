// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/gesture_tracker.h"
#include "pomdog/input/mouse.h"
#include "pomdog/input/mouse_buttons.h"
#include "pomdog/input/touchscreen.h"

namespace pomdog {

GestureState
getPrimaryGestureState(const Mouse& mouse, raw_ptr<const Touchscreen> touchscreen) noexcept
{
    if ((touchscreen != nullptr) && touchscreen->isPrimaryTouchPressed()) {
        return GestureState{
            .position = touchscreen->getPrimaryTouchPosition(),
            .pressed = true,
        };
    }
    return GestureState{
        .position = mouse.getPosition(),
        .pressed = mouse.isButtonDown(MouseButtons::Left),
    };
}

void GestureTracker::update(const GestureState& state) noexcept
{
    previousPosition_ = position_;
    previousPressed_ = pressed_;
    position_ = state.position;
    pressed_ = state.pressed;
}

Point2D
GestureTracker::getPosition() const noexcept
{
    return position_;
}

bool GestureTracker::isPressed() const noexcept
{
    return pressed_;
}

bool GestureTracker::pressedThisFrame() const noexcept
{
    return pressed_ && !previousPressed_;
}

bool GestureTracker::releasedThisFrame() const noexcept
{
    return !pressed_ && previousPressed_;
}

Point2D
GestureTracker::getDragDelta() const noexcept
{
    if (pressed_ && previousPressed_) {
        return position_ - previousPosition_;
    }
    return Point2D{0, 0};
}

} // namespace pomdog
