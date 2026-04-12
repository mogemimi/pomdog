// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/input/backends/mouse_state.h"
#include "pomdog/input/mouse.h"

namespace pomdog::detail {

/// Platform-independent Mouse implementation that holds the current
/// mouse state. Platform backends forward events into this class
/// via its setters.
class POMDOG_EXPORT MouseImpl final : public Mouse {
private:
    MouseState mouseState_;

public:
    MouseImpl();

    MouseImpl(const MouseImpl&) = delete;
    MouseImpl& operator=(const MouseImpl&) = delete;

    /// Gets the position of the mouse cursor.
    [[nodiscard]] Point2D getPosition() const noexcept override;

    /// Determines whether the specified mouse button is pressed.
    [[nodiscard]] bool isButtonDown(MouseButtons button) const noexcept override;

    /// Gets horizontal mouse scroll (accumulated).
    [[nodiscard]] i32 getScrollX() const noexcept override;

    /// Gets vertical mouse scroll (accumulated).
    [[nodiscard]] i32 getScrollY() const noexcept override;

    /// Returns true if a mouse is present, false otherwise.
    [[nodiscard]] bool isPresent() const noexcept override;

    /// Sets the cursor position.
    void setPosition(const Point2D& position) noexcept;

    /// Sets the state of the specified button.
    void setButton(MouseButtons button, ButtonState state) noexcept;

    /// Sets the vertical scroll wheel value.
    void setScrollWheel(i32 value) noexcept;

    /// Adds a delta to the vertical scroll wheel value.
    void addScrollWheel(i32 delta) noexcept;

    /// Resets all button states to released.
    void clearAllButtons() noexcept;
};

} // namespace pomdog::detail
