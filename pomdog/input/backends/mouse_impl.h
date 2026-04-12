// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/input/mouse.h"
#include "pomdog/input/mouse_buttons.h"
#include "pomdog/math/point2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <bitset>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
enum class ButtonState : u8;
} // namespace pomdog

namespace pomdog::detail {

/// Platform-independent Mouse implementation that holds the current
/// mouse state. Platform backends forward events into this class
/// via its setters.
class POMDOG_EXPORT MouseImpl final : public Mouse {
private:
    Point2D position_{0, 0};
    f64 scrollX_ = 0.0;
    f64 scrollY_ = 0.0;
    f64 scrollDeltaX_ = 0.0;
    f64 scrollDeltaY_ = 0.0;
    std::bitset<5> buttons_;
    bool present_ = false;

public:
    MouseImpl();

    MouseImpl(const MouseImpl&) = delete;
    MouseImpl& operator=(const MouseImpl&) = delete;

    /// Gets the position of the mouse cursor.
    [[nodiscard]] Point2D
    getPosition() const noexcept override;

    /// Determines whether the specified mouse button is pressed.
    [[nodiscard]] bool
    isButtonDown(MouseButtons button) const noexcept override;

    /// Gets accumulated horizontal scroll value.
    [[nodiscard]] f64
    getScrollX() const noexcept override;

    /// Gets accumulated vertical scroll value.
    [[nodiscard]] f64
    getScrollY() const noexcept override;

    /// Gets the horizontal scroll delta for the current frame.
    [[nodiscard]] f64
    getScrollDeltaX() const noexcept override;

    /// Gets the vertical scroll delta for the current frame.
    [[nodiscard]] f64
    getScrollDeltaY() const noexcept override;

    /// Returns true if the mouse cursor is inside the window, false otherwise.
    [[nodiscard]] bool
    isPresent() const noexcept override;

    /// Sets the cursor position.
    void setPosition(const Point2D& position) noexcept;

    /// Sets the state of the specified button.
    void setButton(MouseButtons button, ButtonState state) noexcept;

    /// Adds a delta to the horizontal and vertical scroll values.
    void addScroll(f64 deltaX, f64 deltaY) noexcept;

    /// Sets whether the mouse cursor is inside the window.
    void setPresent(bool present) noexcept;

    /// Resets all button states to released.
    void clearAllButtons() noexcept;

    /// Resets the per-frame scroll deltas to zero.
    void clearScrollDelta() noexcept;
};

} // namespace pomdog::detail
