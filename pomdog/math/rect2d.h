// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/point2d.h"

namespace pomdog {

/// Rect2D is a rectangle in 2D space.
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
class POMDOG_EXPORT Rect2D final {
public:
    i32 x;
    i32 y;
    i32 width;
    i32 height;

public:
    Rect2D() noexcept = default;

    Rect2D(
        i32 x,
        i32 y,
        i32 width,
        i32 height) noexcept;

    Rect2D(
        const Point2D& position,
        i32 width,
        i32 height) noexcept;

    [[nodiscard]] bool operator==(const Rect2D&) const noexcept;
    [[nodiscard]] bool operator!=(const Rect2D&) const noexcept;

    /// Returns the minimum x coordinate.
    [[nodiscard]] i32
    minX() const noexcept;

    /// Returns the maximum x coordinate (x + width).
    [[nodiscard]] i32
    maxX() const noexcept;

    /// Returns the minimum y coordinate.
    [[nodiscard]] i32
    minY() const noexcept;

    /// Returns the maximum y coordinate (y + height).
    [[nodiscard]] i32
    maxY() const noexcept;

    /// Returns the center point of the rectangle.
    [[nodiscard]] Point2D
    getCenter() const;

    /// Returns the location (x, y) of the rectangle.
    [[nodiscard]] Point2D
    getLocation() const;

    /// Sets the location (x, y) of the rectangle.
    void setLocation(const Point2D& position);

    /// Inflates the rectangle by the specified amounts.
    void inflate(i32 horizontalAmount, i32 verticalAmount);

    /// Offsets the rectangle by the specified amounts.
    void offset(i32 offsetX, i32 offsetY);

    /// Offsets the rectangle by the specified point.
    void offset(const Point2D& offset);

    /// Returns true if the specified point is inside the rectangle.
    [[nodiscard]] bool
    contains(i32 x, i32 y) const noexcept;

    /// Returns true if the specified point is inside the rectangle.
    [[nodiscard]] bool
    contains(const Point2D& point) const noexcept;

    /// Returns true if the specified rectangle is entirely inside this rectangle.
    [[nodiscard]] bool
    contains(const Rect2D& rectangle) const noexcept;

    /// Returns true if the specified rectangle intersects with this rectangle.
    [[nodiscard]] bool
    intersects(const Rect2D& rectangle) const noexcept;
};

} // namespace pomdog
