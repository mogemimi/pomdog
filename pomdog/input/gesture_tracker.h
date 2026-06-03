// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/point2d.h"
#include "pomdog/memory/raw_ptr.h"

namespace pomdog {
class Mouse;
class Touchscreen;
} // namespace pomdog

namespace pomdog {

/// GestureState is a unified per-frame snapshot of a single pointer that may
/// originate from either a mouse or a touchscreen.
struct POMDOG_EXPORT GestureState final {
    /// The pointer position in logical pixels.
    Point2D position = {};

    /// Whether the pointer is currently pressed (the mouse left button is held
    /// down or a touch point is active).
    bool pressed = false;
};

/// Returns the primary gesture state by combining a mouse and an optional
/// touchscreen.
///
/// A pressed primary touch takes priority; otherwise the mouse position and
/// left-button state are used. `touchscreen` may be null on platforms without
/// touch input.
[[nodiscard]] POMDOG_EXPORT GestureState
getPrimaryGestureState(const Mouse& mouse, raw_ptr<const Touchscreen> touchscreen) noexcept;

/// GestureTracker reports press/release edges and drag deltas for a single
/// pointer across frames, unifying mouse and touch input.
///
/// Feed it one GestureState per frame via `update()`. It is used for tap
/// detection (`pressedThisFrame()` / `releasedThisFrame()`) and for drag /
/// swipe gestures such as scrolling (`getDragDelta()`).
class POMDOG_EXPORT GestureTracker final {
private:
    Point2D position_ = {};
    Point2D previousPosition_ = {};
    bool pressed_ = false;
    bool previousPressed_ = false;

public:
    /// Updates the tracker with this frame's pointer state.
    void
    update(const GestureState& state) noexcept;

    /// Returns the current pointer position in logical pixels.
    [[nodiscard]] Point2D
    getPosition() const noexcept;

    /// Returns true while the pointer is pressed.
    [[nodiscard]] bool
    isPressed() const noexcept;

    /// Returns true only on the frame the pointer transitions to pressed.
    [[nodiscard]] bool
    pressedThisFrame() const noexcept;

    /// Returns true only on the frame the pointer transitions to released.
    [[nodiscard]] bool
    releasedThisFrame() const noexcept;

    /// Returns the pointer movement since the previous frame while pressed, in
    /// logical pixels.
    ///
    /// Returns a zero vector when the pointer is not pressed, or on the first
    /// frame it becomes pressed, so a tap does not produce a spurious delta.
    [[nodiscard]] Point2D
    getDragDelta() const noexcept;
};

} // namespace pomdog
