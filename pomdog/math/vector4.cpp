// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/vector4.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/vector3.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Vector4::Vector4() noexcept = default;

Vector4::Vector4(const Vector3& xyz, float wIn) noexcept
    : x(xyz.x)
    , y(xyz.y)
    , z(xyz.z)
    , w(wIn)
{
}

Vector4::Vector4(float xIn, float yIn, float zIn, float wIn) noexcept
    : x(xIn)
    , y(yIn)
    , z(zIn)
    , w(wIn)
{
}

Vector4& Vector4::operator+=(const Vector4& other) noexcept
{
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;
    return *this;
}

Vector4& Vector4::operator-=(const Vector4& other) noexcept
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;
    return *this;
}

Vector4& Vector4::operator*=(float factor) noexcept
{
    x *= factor;
    y *= factor;
    z *= factor;
    w *= factor;
    return *this;
}

Vector4& Vector4::operator/=(float factor) noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    x /= factor;
    y /= factor;
    z /= factor;
    w /= factor;
    return *this;
}

Vector4 Vector4::operator+() const noexcept
{
    return *this;
}

Vector4 Vector4::operator-() const noexcept
{
    return Vector4{-x, -y, -z, -w};
}

Vector4 Vector4::operator+(const Vector4& other) const noexcept
{
    return Vector4{
        x + other.x,
        y + other.y,
        z + other.z,
        w + other.w,
    };
}

Vector4 Vector4::operator-(const Vector4& other) const noexcept
{
    return Vector4{
        x - other.x,
        y - other.y,
        z - other.z,
        w - other.w,
    };
}

Vector4 Vector4::operator*(const Vector4& other) const noexcept
{
    return Vector4{
        x * other.x,
        y * other.y,
        z * other.z,
        w * other.w,
    };
}

Vector4 Vector4::operator/(const Vector4& other) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(other.x) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(other.y) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(other.z) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(other.w) != FP_ZERO);
    return Vector4{
        x / other.x,
        y / other.y,
        z / other.z,
        w / other.w,
    };
}

Vector4 Vector4::operator*(float factor) const noexcept
{
    return Vector4{
        x * factor,
        y * factor,
        z * factor,
        w * factor,
    };
}

Vector4 Vector4::operator/(float factor) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    return Vector4{
        x / factor,
        y / factor,
        z / factor,
        w / factor,
    };
}

bool Vector4::operator==(const Vector4& other) const noexcept
{
    return (x == other.x) && (y == other.y) && (z == other.z) && (w == other.w);
}

bool Vector4::operator!=(const Vector4& other) const noexcept
{
    return (x != other.x) || (y != other.y) || (z != other.z) || (w != other.w);
}

const float* Vector4::data() const noexcept
{
    return &x;
}

float* Vector4::data() noexcept
{
    return &x;
}

[[nodiscard]] Vector4
operator*(float factor, const Vector4& vector) noexcept
{
    return Vector4{
        factor * vector.x,
        factor * vector.y,
        factor * vector.z,
        factor * vector.w,
    };
}

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] float
length(const Vector4& v) noexcept
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

[[nodiscard]] float
lengthSquared(const Vector4& v) noexcept
{
    return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
}

[[nodiscard]] float
distance(const Vector4& a, const Vector4& b) noexcept
{
    return math::length(a - b);
}

[[nodiscard]] float
distanceSquared(const Vector4& a, const Vector4& b) noexcept
{
    return math::lengthSquared(a - b);
}

[[nodiscard]] float
dot(const Vector4& a, const Vector4& b) noexcept
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

[[nodiscard]] Vector4
normalize(const Vector4& source) noexcept
{
    const auto length = math::length(source);

    if (length > std::numeric_limits<decltype(length)>::epsilon()) {
        POMDOG_ASSERT(std::fpclassify(length) != FP_ZERO);
        POMDOG_ASSERT(std::fpclassify(length) != FP_NAN);
        POMDOG_ASSERT(std::fpclassify(length) != FP_INFINITE);
        const auto inverseLength = 1.0f / length;
        return source * inverseLength;
    }
    return source;
}

[[nodiscard]] Vector4
transform(const Vector4& vector, const Matrix4x4& matrix) noexcept
{
    return Vector4{
        (vector.x * matrix.m[0][0]) + (vector.y * matrix.m[1][0]) + (vector.z * matrix.m[2][0]) + (vector.w * matrix.m[3][0]),
        (vector.x * matrix.m[0][1]) + (vector.y * matrix.m[1][1]) + (vector.z * matrix.m[2][1]) + (vector.w * matrix.m[3][1]),
        (vector.x * matrix.m[0][2]) + (vector.y * matrix.m[1][2]) + (vector.z * matrix.m[2][2]) + (vector.w * matrix.m[3][2]),
        (vector.x * matrix.m[0][3]) + (vector.y * matrix.m[1][3]) + (vector.z * matrix.m[2][3]) + (vector.w * matrix.m[3][3]),
    };
}

} // namespace pomdog::math
