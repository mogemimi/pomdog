// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/point3d.h"
#include "pomdog/math/vector3.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Point3D::Point3D() noexcept = default;

Point3D::Point3D(std::int32_t xIn, std::int32_t yIn, std::int32_t zIn) noexcept
    : x(xIn)
    , y(yIn)
    , z(zIn)
{
}

Point3D& Point3D::operator+=(const Point3D& other) noexcept
{
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Point3D& Point3D::operator-=(const Point3D& other) noexcept
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Point3D& Point3D::operator*=(std::int32_t factor) noexcept
{
    x *= factor;
    y *= factor;
    z *= factor;
    return *this;
}

Point3D& Point3D::operator/=(std::int32_t factor) noexcept
{
    POMDOG_ASSERT(factor != 0);
    x /= factor;
    y /= factor;
    z /= factor;
    return *this;
}

Point3D Point3D::operator+() const noexcept
{
    return *this;
}

Point3D Point3D::operator-() const noexcept
{
    return Point3D{-x, -y, -z};
}

Point3D Point3D::operator+(const Point3D& other) const noexcept
{
    return Point3D{
        x + other.x,
        y + other.y,
        z + other.z,
    };
}

Point3D Point3D::operator-(const Point3D& other) const noexcept
{
    return Point3D{
        x - other.x,
        y - other.y,
        z - other.z,
    };
}

Point3D Point3D::operator*(const Point3D& other) const noexcept
{
    return Point3D{
        x * other.x,
        y * other.y,
        z * other.z,
    };
}

Point3D Point3D::operator/(const Point3D& other) const
{
    POMDOG_ASSERT(other.x != 0);
    POMDOG_ASSERT(other.y != 0);
    POMDOG_ASSERT(other.z != 0);
    return Point3D{
        x / other.x,
        y / other.y,
        z / other.z,
    };
}

Point3D Point3D::operator*(std::int32_t factor) const noexcept
{
    return Point3D{
        x * factor,
        y * factor,
        z * factor,
    };
}

Point3D Point3D::operator/(std::int32_t factor) const noexcept
{
    POMDOG_ASSERT(factor != 0);
    return Point3D{
        x / factor,
        y / factor,
        z / factor,
    };
}

bool Point3D::operator==(const Point3D& other) const noexcept
{
    return ((x == other.x) && (y == other.y) && (z == other.z));
}

bool Point3D::operator!=(const Point3D& other) const noexcept
{
    return ((x != other.x) || (y != other.y) || (z != other.z));
}

Point3D Point3D::Zero() noexcept
{
    return Point3D{0, 0, 0};
}

[[nodiscard]] Point3D
operator*(std::int32_t factor, const Point3D& coordinate) noexcept
{
    return Point3D{
        factor * coordinate.x,
        factor * coordinate.y,
        factor * coordinate.z,
    };
}

} // namespace pomdog

namespace pomdog::math {

Point3D ToPoint3D(const Vector3& vec) noexcept
{
    return Point3D{
        static_cast<std::int32_t>(vec.x),
        static_cast<std::int32_t>(vec.y),
        static_cast<std::int32_t>(vec.z),
    };
}

Vector3 ToVector3(const Point3D& point) noexcept
{
    return Vector3{
        static_cast<float>(point.x),
        static_cast<float>(point.y),
        static_cast<float>(point.z),
    };
}

Point3D Abs(const Point3D& point) noexcept
{
    return Point3D{
        std::abs(point.x),
        std::abs(point.y),
        std::abs(point.z),
    };
}

} // namespace pomdog::math
