// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/point2d.hpp"
#include "pomdog/utility/assert.hpp"

namespace pomdog {

Point2D const Point2D::Zero = {0, 0};

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

Point2D& Point2D::operator*=(std::int32_t scaleFactor) noexcept
{
    X *= scaleFactor;
    Y *= scaleFactor;
    return *this;
}

Point2D& Point2D::operator/=(std::int32_t scaleFactor) noexcept
{
    POMDOG_ASSERT(scaleFactor != 0);
    X /= scaleFactor;
    Y /= scaleFactor;
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

Point2D Point2D::operator*(std::int32_t scaleFactor) const noexcept
{
    return {X * scaleFactor, Y * scaleFactor};
}

Point2D Point2D::operator/(std::int32_t scaleFactor) const noexcept
{
    POMDOG_ASSERT(scaleFactor != 0);
    return {X / scaleFactor, Y / scaleFactor};
}

bool Point2D::operator==(const Point2D& other) const noexcept
{
    return (X == other.X && Y == other.Y);
}

bool Point2D::operator!=(const Point2D& other) const noexcept
{
    return (X != other.X || Y != other.Y);
}

[[nodiscard]] Point2D
operator*(std::int32_t scaleFactor, const Point2D& coordinate) noexcept
{
    return {scaleFactor * coordinate.X, scaleFactor * coordinate.Y};
}

} // namespace pomdog
