// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/input/mouse_buttons.h"
#include "pomdog/math/point2d.h"

namespace pomdog {

/// Represents the current state of a mouse device.
class POMDOG_EXPORT Mouse {
public:
    Mouse() noexcept;

    Mouse(const Mouse&) = delete;
    Mouse& operator=(const Mouse&) = delete;

    virtual ~Mouse();

    /// Gets the position of the mouse cursor.
    [[nodiscard]] virtual Point2D
    getPosition() const noexcept = 0;

    /// Determines whether the specified mouse button is pressed.
    [[nodiscard]] virtual bool
    isButtonDown(MouseButtons button) const noexcept = 0;

    /// Gets accumulated horizontal scroll value.
    /// The unit is normalized so that 1.0 equals one scroll notch.
    [[nodiscard]] virtual f64
    getScrollX() const noexcept = 0;

    /// Gets accumulated vertical scroll value.
    /// The unit is normalized so that 1.0 equals one scroll notch.
    [[nodiscard]] virtual f64
    getScrollY() const noexcept = 0;

    /// Gets the horizontal scroll delta for the current frame.
    [[nodiscard]] virtual f64
    getScrollDeltaX() const noexcept = 0;

    /// Gets the vertical scroll delta for the current frame.
    [[nodiscard]] virtual f64
    getScrollDeltaY() const noexcept = 0;

    /// Returns true if the mouse cursor is inside the window, false otherwise.
    [[nodiscard]] virtual bool
    isPresent() const noexcept = 0;
};

} // namespace pomdog
