// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/point2d.h"
#include "pomdog/math/vector2.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Point2D::Point2D() noexcept = default;

Point2D::Point2D(std::int32_t xIn, std::int32_t yIn) noexcept
    : x(xIn)
    , y(yIn)
{
}

Point2D& Point2D::operator+=(const Point2D& other) noexcept
{
    x += other.x;
    y += other.y;
    return *this;
}

Point2D& Point2D::operator-=(const Point2D& other) noexcept
{
    x -= other.x;
    y -= other.y;
    return *this;
}

Point2D& Point2D::operator*=(std::int32_t factor) noexcept
{
    x *= factor;
    y *= factor;
    return *this;
}

Point2D& Point2D::operator/=(std::int32_t factor) noexcept
{
    POMDOG_ASSERT(factor != 0);
    x /= factor;
    y /= factor;
    return *this;
}

Point2D Point2D::operator+() const noexcept
{
    return *this;
}

Point2D Point2D::operator-() const noexcept
{
    return Point2D{-x, -y};
}

Point2D Point2D::operator+(const Point2D& other) const noexcept
{
    return Point2D{
        x + other.x,
        y + other.y,
    };
}

Point2D Point2D::operator-(const Point2D& other) const noexcept
{
    return Point2D{
        x - other.x,
        y - other.y,
    };
}

Point2D Point2D::operator*(const Point2D& other) const noexcept
{
    return Point2D{
        x * other.x,
        y * other.y,
    };
}

Point2D Point2D::operator/(const Point2D& other) const noexcept
{
    return Point2D{
        x / other.x,
        y / other.y,
    };
}

Point2D Point2D::operator*(std::int32_t factor) const noexcept
{
    return Point2D{
        x * factor,
        y * factor,
    };
}

Point2D Point2D::operator/(std::int32_t factor) const noexcept
{
    POMDOG_ASSERT(factor != 0);
    return Point2D{
        x / factor,
        y / factor,
    };
}

bool Point2D::operator==(const Point2D& other) const noexcept
{
    return ((x == other.x) && (y == other.y));
}

bool Point2D::operator!=(const Point2D& other) const noexcept
{
    return ((x != other.x) || (y != other.y));
}

Point2D Point2D::Zero() noexcept
{
    return Point2D{0, 0};
}

[[nodiscard]] Point2D
operator*(std::int32_t factor, const Point2D& coordinate) noexcept
{
    return Point2D{
        factor * coordinate.x,
        factor * coordinate.y,
    };
}

} // namespace pomdog

namespace pomdog::math {

Point2D ToPoint2D(const Vector2& vec) noexcept
{
    return Point2D{
        static_cast<std::int32_t>(vec.x),
        static_cast<std::int32_t>(vec.y),
    };
}

Vector2 ToVector2(const Point2D& point) noexcept
{
    return Vector2{
        static_cast<float>(point.x),
        static_cast<float>(point.y),
    };
}

Point2D Abs(const Point2D& point) noexcept
{
    return Point2D{
        std::abs(point.x),
        std::abs(point.y),
    };
}

} // namespace pomdog::math
