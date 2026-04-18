// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/point_xz.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Point2D;
} // namespace pomdog

namespace pomdog {

/// RectXZ is a rectangle in 2-dimensional XZ space, defined by min and max points.
///
/// ```
///  minX                    maxX
///   │          dx           │
///   │◄─────────────────────►│
///
///   ┌───────────────────────┐ ─── maxZ
///   │                       │ ▲
///   │                       │ │
///   │                       │ │
///   │           *           │ │ dz
///   │         center        │ │
///   │                       │ │
///   │                       │ ▼
///   └───────────────────────┘ ─── minZ
/// (x, z)
/// ```
class POMDOG_EXPORT RectXZ final {
public:
    PointXZ min;
    PointXZ max;

public:
    [[nodiscard]] bool operator==(const RectXZ& other) const noexcept;
    [[nodiscard]] bool operator!=(const RectXZ& other) const noexcept;

    /// Returns the width of the rectangle (max.x - min.x).
    [[nodiscard]] i32
    dx() const noexcept;

    /// Returns the depth of the rectangle (max.z - min.z).
    [[nodiscard]] i32
    dz() const noexcept;

    /// Returns the minimum x coordinate.
    [[nodiscard]] i32
    minX() const noexcept;

    /// Returns the maximum x coordinate.
    [[nodiscard]] i32
    maxX() const noexcept;

    /// Returns the minimum z coordinate.
    [[nodiscard]] i32
    minZ() const noexcept;

    /// Returns the maximum z coordinate.
    [[nodiscard]] i32
    maxZ() const noexcept;

    /// Returns true if the specified point is inside the rectangle.
    [[nodiscard]] bool
    contains(i32 x, i32 z) const noexcept;

    /// Returns true if the specified Point2D is inside the rectangle.
    [[nodiscard]] bool
    contains(const Point2D& point) const noexcept;

    /// Returns true if the specified PointXZ is inside the rectangle.
    [[nodiscard]] bool
    contains(const PointXZ& point) const noexcept;

    /// Returns true if the specified rectangle intersects this rectangle.
    [[nodiscard]] bool
    intersects(RectXZ bounds) const noexcept;

    /// Returns the overlap region between two rectangles.
    [[nodiscard]] std::optional<RectXZ>
    overlap(RectXZ bounds) const noexcept;
};

} // namespace pomdog
