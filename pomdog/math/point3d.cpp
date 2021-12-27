// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/point3d.h"
#include "pomdog/math/vector3.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

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

Point3D& Point3D::operator*=(std::int32_t factor) noexcept
{
    X *= factor;
    Y *= factor;
    Z *= factor;
    return *this;
}

Point3D& Point3D::operator/=(std::int32_t factor) noexcept
{
    POMDOG_ASSERT(factor != 0);
    X /= factor;
    Y /= factor;
    Z /= factor;
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

Point3D Point3D::operator*(std::int32_t factor) const noexcept
{
    return {X * factor, Y * factor, Z * factor};
}

Point3D Point3D::operator/(std::int32_t factor) const noexcept
{
    POMDOG_ASSERT(factor != 0);
    return {X / factor, Y / factor, Z / factor};
}

bool Point3D::operator==(const Point3D& other) const noexcept
{
    return (X == other.X && Y == other.Y && Z == other.Z);
}

bool Point3D::operator!=(const Point3D& other) const noexcept
{
    return (X != other.X || Y != other.Y || Z != other.Z);
}

Point3D Point3D::Zero() noexcept
{
    return Point3D{0, 0, 0};
}

[[nodiscard]] Point3D
operator*(std::int32_t factor, const Point3D& coordinate) noexcept
{
    return Point3D{
        factor * coordinate.X,
        factor * coordinate.Y,
        factor * coordinate.Z};
}

} // namespace pomdog

namespace pomdog::math {

Point3D ToPoint3D(const Vector3& vec) noexcept
{
    return Point3D{
        static_cast<std::int32_t>(vec.X),
        static_cast<std::int32_t>(vec.Y),
        static_cast<std::int32_t>(vec.Z),
    };
}

Vector3 ToVector3(const Point3D& point) noexcept
{
    return Vector3{
        static_cast<float>(point.X),
        static_cast<float>(point.Y),
        static_cast<float>(point.Z),
    };
}

Point3D Abs(const Point3D& point) noexcept
{
    return Point3D{
        std::abs(point.X),
        std::abs(point.Y),
        std::abs(point.Z),
    };
}

} // namespace pomdog::math
