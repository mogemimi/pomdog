// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {
class Vector2;
class VectorXZ;
} // namespace pomdog

namespace pomdog {

/// PointXZ is a point in 2-dimensional XZ space.
class POMDOG_EXPORT PointXZ final {
public:
    i32 x;
    i32 z;

public:
    /// Constructor
    PointXZ() noexcept;

    /// Constructs from integer values.
    PointXZ(i32 x, i32 z) noexcept;

    // Assignment operators:
    PointXZ& operator+=(const PointXZ&) noexcept;
    PointXZ& operator-=(const PointXZ&) noexcept;
    PointXZ& operator*=(i32 factor) noexcept;
    PointXZ& operator/=(i32 factor) noexcept;

    // Unary operators:
    [[nodiscard]] PointXZ operator+() const noexcept;
    [[nodiscard]] PointXZ operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] PointXZ operator+(const PointXZ&) const noexcept;
    [[nodiscard]] PointXZ operator-(const PointXZ&) const noexcept;
    [[nodiscard]] PointXZ operator*(const PointXZ&) const noexcept;
    [[nodiscard]] PointXZ operator/(const PointXZ&) const noexcept;
    [[nodiscard]] PointXZ operator*(i32 factor) const noexcept;
    [[nodiscard]] PointXZ operator/(i32 factor) const noexcept;

    [[nodiscard]] bool operator==(const PointXZ&) const noexcept;
    [[nodiscard]] bool operator!=(const PointXZ&) const noexcept;
};

/// Multiplies a point by a scalar factor.
[[nodiscard]] PointXZ POMDOG_EXPORT
operator*(i32 factor, const PointXZ& v) noexcept;

} // namespace pomdog

namespace pomdog::math {

/// Returns the component-wise absolute value of a point.
[[nodiscard]] POMDOG_EXPORT PointXZ
abs(PointXZ v) noexcept;

/// Returns the component-wise minimum of two points.
[[nodiscard]] POMDOG_EXPORT PointXZ
min(PointXZ a, PointXZ b) noexcept;

/// Returns the component-wise maximum of two points.
[[nodiscard]] POMDOG_EXPORT PointXZ
max(PointXZ a, PointXZ b) noexcept;

/// Restricts a point between a minimum and a maximum value.
[[nodiscard]] POMDOG_EXPORT PointXZ
clamp(PointXZ source, PointXZ min, PointXZ max) noexcept;

/// Converts a PointXZ to a Vector2 type.
[[nodiscard]] POMDOG_EXPORT Vector2
toVector2(PointXZ p) noexcept;

/// Converts a PointXZ to a VectorXZ type.
[[nodiscard]] POMDOG_EXPORT VectorXZ
toVectorXZ(PointXZ p) noexcept;

} // namespace pomdog::math
