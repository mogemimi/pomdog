// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Point3D is a point in 3-dimensional space.
class POMDOG_EXPORT Point3D final {
public:
    std::int32_t X;
    std::int32_t Y;
    std::int32_t Z;

public:
    /// Constructors
    Point3D() noexcept;

    /// Constructs from std::int32_t values.
    Point3D(std::int32_t x, std::int32_t y, std::int32_t z) noexcept;

    // Assignment operators:
    Point3D& operator+=(const Point3D&) noexcept;
    Point3D& operator-=(const Point3D&) noexcept;
    Point3D& operator*=(std::int32_t scaleFactor) noexcept;
    Point3D& operator/=(std::int32_t scaleFactor) noexcept;

    // Unary operators:
    Point3D operator+() const noexcept;
    Point3D operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Point3D operator+(const Point3D&) const noexcept;
    [[nodiscard]] Point3D operator-(const Point3D&) const noexcept;
    [[nodiscard]] Point3D operator*(const Point3D&) const noexcept;
    [[nodiscard]] Point3D operator/(const Point3D&) const;
    [[nodiscard]] Point3D operator*(std::int32_t scaleFactor) const noexcept;
    [[nodiscard]] Point3D operator/(std::int32_t scaleFactor) const noexcept;

    [[nodiscard]] bool operator==(const Point3D&) const noexcept;
    [[nodiscard]] bool operator!=(const Point3D&) const noexcept;

    static Point3D const Zero;
};

[[nodiscard]] Point3D POMDOG_EXPORT
operator*(std::int32_t scaleFactor, const Point3D& coordinate) noexcept;

} // namespace pomdog
