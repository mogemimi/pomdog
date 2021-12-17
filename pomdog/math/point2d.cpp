// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/point2d.hpp"
#include "pomdog/math/vector2.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Point2D::Point2D() noexcept = default;

Point2D::Point2D(std::int32_t x, std::int32_t y) noexcept
    : X(x)
    , Y(y)
{
}

Point2D& Point2D::operator+=(const Point2D& other) noexcept
{
    X += other.X;
    Y += other.Y;
    return *this;
}

Point2D& Point2D::operator-=(const Point2D& other) noexcept
{
    X -= other.X;
    Y -= other.Y;
    return *this;
}

Point2D& Point2D::operator*=(std::int32_t factor) noexcept
{
    X *= factor;
    Y *= factor;
    return *this;
}

Point2D& Point2D::operator/=(std::int32_t factor) noexcept
{
    POMDOG_ASSERT(factor != 0);
    X /= factor;
    Y /= factor;
    return *this;
}

Point2D Point2D::operator+() const noexcept
{
    return *this;
}

Point2D Point2D::operator-() const noexcept
{
    return {-X, -Y};
}

Point2D Point2D::operator+(const Point2D& other) const noexcept
{
    return {X + other.X, Y + other.Y};
}

Point2D Point2D::operator-(const Point2D& other) const noexcept
{
    return {X - other.X, Y - other.Y};
}

Point2D Point2D::operator*(const Point2D& other) const noexcept
{
    return {X * other.X, Y * other.Y};
}

Point2D Point2D::operator/(const Point2D& other) const noexcept
{
    return {X / other.X, Y / other.Y};
}

Point2D Point2D::operator*(std::int32_t factor) const noexcept
{
    return {X * factor, Y * factor};
}

Point2D Point2D::operator/(std::int32_t factor) const noexcept
{
    POMDOG_ASSERT(factor != 0);
    return {X / factor, Y / factor};
}

bool Point2D::operator==(const Point2D& other) const noexcept
{
    return (X == other.X && Y == other.Y);
}

bool Point2D::operator!=(const Point2D& other) const noexcept
{
    return (X != other.X || Y != other.Y);
}

Point2D Point2D::Zero() noexcept
{
    return Point2D{0, 0};
}

[[nodiscard]] Point2D
operator*(std::int32_t factor, const Point2D& coordinate) noexcept
{
    return {factor * coordinate.X, factor * coordinate.Y};
}

} // namespace pomdog

namespace pomdog::math {

Point2D ToPoint2D(const Vector2& vec) noexcept
{
    return Point2D{
        static_cast<std::int32_t>(vec.X),
        static_cast<std::int32_t>(vec.Y),
    };
}

Vector2 ToVector2(const Point2D& point) noexcept
{
    return Vector2{
        static_cast<float>(point.X),
        static_cast<float>(point.Y),
    };
}

Point2D Abs(const Point2D& point) noexcept
{
    return Point2D{
        std::abs(point.X),
        std::abs(point.Y),
    };
}

} // namespace pomdog::math
