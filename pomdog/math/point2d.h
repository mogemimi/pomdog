// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/math/forward_declarations.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Point2D is a point in 2-dimensional space.
class POMDOG_EXPORT Point2D final {
public:
    std::int32_t X;
    std::int32_t Y;

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
    Point2D operator+() const noexcept;
    Point2D operator-() const noexcept;

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
    [[nodiscard]] static Point2D Zero() noexcept;
};

/// Multiplies a 2D point by a scalar factor.
[[nodiscard]] Point2D POMDOG_EXPORT
operator*(std::int32_t factor, const Point2D& coordinate) noexcept;

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] POMDOG_EXPORT Point2D
ToPoint2D(const Vector2& vec) noexcept;

[[nodiscard]] POMDOG_EXPORT Vector2
ToVector2(const Point2D& point) noexcept;

[[nodiscard]] POMDOG_EXPORT Point2D
Abs(const Point2D& point) noexcept;

} // namespace pomdog::math
