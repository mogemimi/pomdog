// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/math/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Point3D is a point in 3-dimensional space.
class POMDOG_EXPORT Point3D final {
public:
    std::int32_t x;
    std::int32_t y;
    std::int32_t z;

public:
    /// Constructors
    Point3D() noexcept;

    /// Constructs from std::int32_t values.
    Point3D(std::int32_t x, std::int32_t y, std::int32_t z) noexcept;

    // Assignment operators:
    Point3D& operator+=(const Point3D&) noexcept;
    Point3D& operator-=(const Point3D&) noexcept;
    Point3D& operator*=(std::int32_t factor) noexcept;
    Point3D& operator/=(std::int32_t factor) noexcept;

    // Unary operators:
    [[nodiscard]] Point3D operator+() const noexcept;
    [[nodiscard]] Point3D operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Point3D operator+(const Point3D&) const noexcept;
    [[nodiscard]] Point3D operator-(const Point3D&) const noexcept;
    [[nodiscard]] Point3D operator*(const Point3D&) const noexcept;
    [[nodiscard]] Point3D operator/(const Point3D&) const;
    [[nodiscard]] Point3D operator*(std::int32_t factor) const noexcept;
    [[nodiscard]] Point3D operator/(std::int32_t factor) const noexcept;

    [[nodiscard]] bool operator==(const Point3D&) const noexcept;
    [[nodiscard]] bool operator!=(const Point3D&) const noexcept;

    /// Returns the Point3D (0, 0).
    [[nodiscard]] static Point3D
    createZero() noexcept;
};

/// Multiplies a 3D point by a scalar factor.
[[nodiscard]] Point3D POMDOG_EXPORT
operator*(std::int32_t factor, const Point3D& coordinate) noexcept;

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] POMDOG_EXPORT Point3D
toPoint3D(const Vector3& vec) noexcept;

[[nodiscard]] POMDOG_EXPORT Vector3
toVector3(const Point3D& point) noexcept;

[[nodiscard]] POMDOG_EXPORT Point3D
abs(const Point3D& point) noexcept;

} // namespace pomdog::math
