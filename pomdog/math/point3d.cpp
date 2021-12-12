// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/point3d.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/utility/assert.hpp"

namespace pomdog {

Point3D const Point3D::Zero = {0, 0, 0};

Point3D::Point3D() noexcept = default;

Point3D::Point3D(std::int32_t x, std::int32_t y, std::int32_t z) noexcept
    : X(x)
    , Y(y)
    , Z(z)
{
}

Point3D& Point3D::operator+=(const Point3D& other) noexcept
{
    X += other.X;
    Y += other.Y;
    Z += other.Z;
    return *this;
}

Point3D& Point3D::operator-=(const Point3D& other) noexcept
{
    X -= other.X;
    Y -= other.Y;
    Z -= other.Z;
    return *this;
}

Point3D& Point3D::operator*=(std::int32_t scaleFactor) noexcept
{
    X *= scaleFactor;
    Y *= scaleFactor;
    Z *= scaleFactor;
    return *this;
}

Point3D& Point3D::operator/=(std::int32_t scaleFactor) noexcept
{
    POMDOG_ASSERT(scaleFactor != 0);
    X /= scaleFactor;
    Y /= scaleFactor;
    Z /= scaleFactor;
    return *this;
}

Point3D Point3D::operator+() const noexcept
{
    return *this;
}

Point3D Point3D::operator-() const noexcept
{
    return {-X, -Y, -Z};
}

Point3D Point3D::operator+(const Point3D& other) const noexcept
{
    return {X + other.X, Y + other.Y, Z + other.Z};
}

Point3D Point3D::operator-(const Point3D& other) const noexcept
{
    return {X - other.X, Y - other.Y, Z - other.Z};
}

Point3D Point3D::operator*(const Point3D& other) const noexcept
{
    return {X * other.X, Y * other.Y, Z * other.Z};
}

Point3D Point3D::operator/(const Point3D& other) const
{
    POMDOG_ASSERT(other.X != 0);
    POMDOG_ASSERT(other.Y != 0);
    POMDOG_ASSERT(other.Z != 0);
    return {X / other.X, Y / other.Y, Z / other.Z};
}

Point3D Point3D::operator*(std::int32_t scaleFactor) const noexcept
{
    return {X * scaleFactor, Y * scaleFactor, Z * scaleFactor};
}

Point3D Point3D::operator/(std::int32_t scaleFactor) const noexcept
{
    POMDOG_ASSERT(scaleFactor != 0);
    return {X / scaleFactor, Y / scaleFactor, Z / scaleFactor};
}

bool Point3D::operator==(const Point3D& other) const noexcept
{
    return (X == other.X && Y == other.Y && Z == other.Z);
}

bool Point3D::operator!=(const Point3D& other) const noexcept
{
    return (X != other.X || Y != other.Y || Z != other.Z);
}

[[nodiscard]] Point3D
operator*(std::int32_t scaleFactor, const Point3D& coordinate) noexcept
{
    return Point3D{
        scaleFactor * coordinate.X,
        scaleFactor * coordinate.Y,
        scaleFactor * coordinate.Z};
}

} // namespace pomdog
