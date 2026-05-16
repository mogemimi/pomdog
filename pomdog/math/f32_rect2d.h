// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {
class Vector2;
} // namespace pomdog

namespace pomdog {

/// F32Rect2D is a rectangle in 2D space using 32-bit floating-point values.
///
/// ```
///  minX                    maxX
///   │         width         │
///   │◄─────────────────────►│
///
///   ┌───────────────────────┐ ─── maxY
///   │                       │ ▲
///   │                       │ │
///   │                       │ │
///   │           *           │ │ height
///   │         center        │ │
///   │                       │ │
///   │                       │ ▼
///   └───────────────────────┘ ─── minY
/// (x, y)
/// ```
class POMDOG_EXPORT F32Rect2D final {
public:
    f32 x;
    f32 y;
    f32 width;
    f32 height;

public:
    /// Returns the minimum x coordinate.
    [[nodiscard]] f32
    minX() const noexcept;

    /// Returns the maximum x coordinate (x + width).
    [[nodiscard]] f32
    maxX() const noexcept;

    /// Returns the minimum y coordinate.
    [[nodiscard]] f32
    minY() const noexcept;

    /// Returns the maximum y coordinate (y + height).
    [[nodiscard]] f32
    maxY() const noexcept;

    /// Returns the center point of the rectangle.
    [[nodiscard]] Vector2
    center() const noexcept;

    /// Returns the location (x, y) of the rectangle as a Vector2.
    [[nodiscard]] Vector2
    location() const noexcept;

    /// Returns true if the specified point is inside the rectangle.
    [[nodiscard]] bool
    contains(f32 pointX, f32 pointY) const noexcept;

    /// Returns true if the specified point is inside the rectangle.
    [[nodiscard]] bool
    contains(const Vector2& point) const noexcept;

    /// Creates a rectangle from minimum and maximum points.
    [[nodiscard]] static F32Rect2D
    createFromMinMax(Vector2 min, Vector2 max);
};

} // namespace pomdog

namespace pomdog::math {

/// Returns true if two rectangles are approximately equal within the specified tolerance.
[[nodiscard]] POMDOG_EXPORT bool
approxEqual(const F32Rect2D& a, const F32Rect2D& b, f32 tolerance) noexcept;

/// Returns true if two rectangles are approximately equal.
[[nodiscard]] POMDOG_EXPORT bool
approxEqual(const F32Rect2D& a, const F32Rect2D& b) noexcept;

} // namespace pomdog::math
