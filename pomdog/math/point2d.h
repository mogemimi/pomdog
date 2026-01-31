// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {
class Vector2;
} // namespace pomdog

namespace pomdog {

/// Point2D is a point in 2-dimensional space.
class POMDOG_EXPORT Point2D final {
public:
    std::int32_t x;
    std::int32_t y;

public:
    /// Constructor
    Point2D() noexcept;

    /// Constructs from std::int32_t values.
    Point2D(std::int32_t x, std::int32_t y) noexcept;

    // Assignment operators:
    Point2D& operator+=(const Point2D&) noexcept;
    Point2D& operator-=(const Point2D&) noexcept;
    Point2D& operator*=(std::int32_t factor) noexcept;
    Point2D& operator/=(std::int32_t factor) noexcept;

    // Unary operators:
    [[nodiscard]] Point2D operator+() const noexcept;
    [[nodiscard]] Point2D operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Point2D operator+(const Point2D&) const noexcept;
    [[nodiscard]] Point2D operator-(const Point2D&) const noexcept;
    [[nodiscard]] Point2D operator*(const Point2D&) const noexcept;
    [[nodiscard]] Point2D operator/(const Point2D&) const noexcept;
    [[nodiscard]] Point2D operator*(std::int32_t factor) const noexcept;
    [[nodiscard]] Point2D operator/(std::int32_t factor) const noexcept;

    [[nodiscard]] bool operator==(const Point2D&) const noexcept;
    [[nodiscard]] bool operator!=(const Point2D&) const noexcept;

    /// Returns the Point2D (0, 0).
    [[nodiscard]] static Point2D
    createZero() noexcept;
};

/// Multiplies a 2D point by a scalar factor.
[[nodiscard]] Point2D POMDOG_EXPORT
operator*(std::int32_t factor, const Point2D& coordinate) noexcept;

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] POMDOG_EXPORT Point2D
toPoint2D(const Vector2& vec) noexcept;

[[nodiscard]] POMDOG_EXPORT Vector2
toVector2(const Point2D& point) noexcept;

[[nodiscard]] POMDOG_EXPORT Point2D
abs(const Point2D& point) noexcept;

} // namespace pomdog::math
