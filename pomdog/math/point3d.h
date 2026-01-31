// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {
class Vector3;
} // namespace pomdog

namespace pomdog {

/// Point3D is a point in 3-dimensional space.
class POMDOG_EXPORT Point3D final {
public:
    i32 x;
    i32 y;
    i32 z;

public:
    /// Constructors
    Point3D() noexcept;

    /// Constructs from integer values.
    Point3D(i32 x, i32 y, i32 z) noexcept;

    // Assignment operators:
    Point3D& operator+=(const Point3D&) noexcept;
    Point3D& operator-=(const Point3D&) noexcept;
    Point3D& operator*=(i32 factor) noexcept;
    Point3D& operator/=(i32 factor) noexcept;

    // Unary operators:
    [[nodiscard]] Point3D operator+() const noexcept;
    [[nodiscard]] Point3D operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Point3D operator+(const Point3D&) const noexcept;
    [[nodiscard]] Point3D operator-(const Point3D&) const noexcept;
    [[nodiscard]] Point3D operator*(const Point3D&) const noexcept;
    [[nodiscard]] Point3D operator/(const Point3D&) const;
    [[nodiscard]] Point3D operator*(i32 factor) const noexcept;
    [[nodiscard]] Point3D operator/(i32 factor) const noexcept;

    [[nodiscard]] bool operator==(const Point3D&) const noexcept;
    [[nodiscard]] bool operator!=(const Point3D&) const noexcept;

    /// Returns the Point3D (0, 0).
    [[nodiscard]] static Point3D
    createZero() noexcept;
};

/// Multiplies a 3D point by a scalar factor.
[[nodiscard]] Point3D POMDOG_EXPORT
operator*(i32 factor, const Point3D& coordinate) noexcept;

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] POMDOG_EXPORT Point3D
toPoint3D(const Vector3& vec) noexcept;

[[nodiscard]] POMDOG_EXPORT Vector3
toVector3(const Point3D& point) noexcept;

[[nodiscard]] POMDOG_EXPORT Point3D
abs(const Point3D& point) noexcept;

} // namespace pomdog::math
