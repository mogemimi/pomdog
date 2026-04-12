// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/point2d.h"

namespace pomdog {

/// Represents a touchscreen input device.
class POMDOG_EXPORT Touchscreen {
public:
    Touchscreen() noexcept;

    Touchscreen(const Touchscreen&) = delete;
    Touchscreen& operator=(const Touchscreen&) = delete;

    virtual ~Touchscreen();

    /// Returns true if mouse simulation is enabled, false otherwise.
    [[nodiscard]] virtual bool
    isMouseSimulationEnabled() const noexcept = 0;

    /// Enables or disables mouse simulation from touch input.
    virtual void
    setMouseSimulationEnabled(bool enabled) noexcept = 0;

    /// Returns the maximum number of simultaneous touch points supported.
    [[nodiscard]] virtual u8
    getSupportedTouchCount() const noexcept = 0;

    /// Gets the position of the specified touch point.
    [[nodiscard]] virtual Point2D
    getPosition(u8 touchIndex) const noexcept = 0;

    /// Returns true if the specified touch point is currently pressed.
    [[nodiscard]] virtual bool
    isTouchPressed(u8 touchIndex) const noexcept = 0;

    /// Gets the position of the primary (first) touch point.
    [[nodiscard]] virtual Point2D
    getPrimaryTouchPosition() const noexcept = 0;

    /// Returns true if the primary (first) touch point is pressed.
    [[nodiscard]] virtual bool
    isPrimaryTouchPressed() const noexcept = 0;
};

} // namespace pomdog
