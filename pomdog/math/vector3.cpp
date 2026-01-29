// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/vector3.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/quaternion.h"
#include "pomdog/math/vector2.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Vector3::Vector3() noexcept = default;

Vector3::Vector3(float xIn, float yIn, float zIn) noexcept
    : x(xIn)
    , y(yIn)
    , z(zIn)
{
}

Vector3::Vector3(const Vector2& vec, float zIn) noexcept
    : x(vec.x)
    , y(vec.y)
    , z(zIn)
{
}

Vector3& Vector3::operator+=(const Vector3& vec) noexcept
{
    x += vec.x;
    y += vec.y;
    z += vec.z;
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& vec) noexcept
{
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;
    return *this;
}

Vector3& Vector3::operator*=(float factor) noexcept
{
    x *= factor;
    y *= factor;
    z *= factor;
    return *this;
}

Vector3& Vector3::operator/=(float factor) noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    x /= factor;
    y /= factor;
    z /= factor;
    return *this;
}

Vector3 Vector3::operator+() const noexcept
{
    return *this;
}

Vector3 Vector3::operator-() const noexcept
{
    return Vector3{-x, -y, -z};
}

Vector3 Vector3::operator+(const Vector3& other) const noexcept
{
    return Vector3{
        x + other.x,
        y + other.y,
        z + other.z};
}

Vector3 Vector3::operator-(const Vector3& other) const noexcept
{
    return Vector3{
        x - other.x,
        y - other.y,
        z - other.z};
}

Vector3 Vector3::operator*(const Vector3& other) const noexcept
{
    return Vector3{
        x * other.x,
        y * other.y,
        z * other.z};
}

Vector3 Vector3::operator/(const Vector3& other) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(other.x) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(other.y) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(other.z) != FP_ZERO);

    return Vector3{
        x / other.x,
        y / other.y,
        z / other.z};
}

Vector3 Vector3::operator*(float factor) const noexcept
{
    return Vector3{
        x * factor,
        y * factor,
        z * factor};
}

Vector3 Vector3::operator/(float factor) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    return Vector3{
        x / factor,
        y / factor,
        z / factor};
}

bool Vector3::operator==(const Vector3& other) const noexcept
{
    return x == other.x && y == other.y && z == other.z;
}

bool Vector3::operator!=(const Vector3& other) const noexcept
{
    return x != other.x || y != other.y || z != other.z;
}

Vector3
Vector3::toEulerAngles(const Quaternion& q)
{
    constexpr float Two = 2;
    const auto xx = q.x * q.x;
    const auto yy = q.y * q.y;
    const auto zz = q.z * q.z;
    const auto ww = q.w * q.w;
    auto roll = std::atan2(Two * (q.w * q.z + q.x * q.y), ww + xx - yy - zz);
    auto yaw = std::asin(Two * (q.w * q.y - q.z * q.x));
    auto pitch = std::atan2(Two * (q.w * q.x + q.y * q.z), ww - xx - yy + zz);
    return Vector3{pitch, yaw, roll};
}

const float* Vector3::data() const noexcept
{
    return &x;
}

float* Vector3::data() noexcept
{
    return &x;
}

Vector3 Vector3::createZero() noexcept
{
    return Vector3{0.0f, 0.0f, 0.0f};
}

Vector3 Vector3::createUnitX() noexcept
{
    return Vector3{1.0f, 0.0f, 0.0f};
}

Vector3 Vector3::createUnitY() noexcept
{
    return Vector3{0.0f, 1.0f, 0.0f};
}

Vector3 Vector3::createUnitZ() noexcept
{
    return Vector3{0.0f, 0.0f, 1.0f};
}

[[nodiscard]] Vector3
operator*(float factor, const Vector3& vector) noexcept
{
    return Vector3{
        factor * vector.x,
        factor * vector.y,
        factor * vector.z};
}

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] float
length(const Vector3& v) noexcept
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

[[nodiscard]] float
lengthSquared(const Vector3& v) noexcept
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

[[nodiscard]] float
distance(const Vector3& a, const Vector3& b) noexcept
{
    return math::length(a - b);
}

[[nodiscard]] float
distanceSquared(const Vector3& a, const Vector3& b) noexcept
{
    return math::lengthSquared(a - b);
}

[[nodiscard]] float
dot(const Vector3& a, const Vector3& b) noexcept
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

[[nodiscard]] Vector3
cross(const Vector3& a, const Vector3& b) noexcept
{
    return Vector3{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
}

[[nodiscard]] Vector3
min(const Vector3& a, const Vector3& b) noexcept
{
    return Vector3{
        std::min(a.x, b.x),
        std::min(a.y, b.y),
        std::min(a.z, b.z),
    };
}

[[nodiscard]] Vector3
max(const Vector3& a, const Vector3& b) noexcept
{
    return Vector3{
        std::max(a.x, b.x),
        std::max(a.y, b.y),
        std::max(a.z, b.z),
    };
}

[[nodiscard]] Vector3
clamp(const Vector3& source, const Vector3& min, const Vector3& max) noexcept
{
    return Vector3{
        math::clamp(source.x, min.x, max.x),
        math::clamp(source.y, min.y, max.y),
        math::clamp(source.z, min.z, max.z),
    };
}

[[nodiscard]] Vector3
lerp(const Vector3& source1, const Vector3& source2, float amount)
{
    return Vector3{
        math::lerp(source1.x, source2.x, amount),
        math::lerp(source1.y, source2.y, amount),
        math::lerp(source1.z, source2.z, amount),
    };
}

[[nodiscard]] Vector3
smoothstep(const Vector3& source1, const Vector3& source2, float amount)
{
    return Vector3{
        math::smoothstep(source1.x, source2.x, amount),
        math::smoothstep(source1.y, source2.y, amount),
        math::smoothstep(source1.z, source2.z, amount),
    };
}

[[nodiscard]] Vector3
normalize(const Vector3& source) noexcept
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

[[nodiscard]] Vector3
transform(const Vector3& position, const Matrix4x4& matrix) noexcept
{
    return Vector3{
        (position.x * matrix.m[0][0]) + (position.y * matrix.m[1][0]) + (position.z * matrix.m[2][0]) + matrix.m[3][0],
        (position.x * matrix.m[0][1]) + (position.y * matrix.m[1][1]) + (position.z * matrix.m[2][1]) + matrix.m[3][1],
        (position.x * matrix.m[0][2]) + (position.y * matrix.m[1][2]) + (position.z * matrix.m[2][2]) + matrix.m[3][2],
    };
}

[[nodiscard]] Vector3
transformNormal(const Vector3& normal, const Matrix4x4& matrix) noexcept
{
    return Vector3{
        (normal.x * matrix.m[0][0]) + (normal.y * matrix.m[1][0]) + (normal.z * matrix.m[2][0]),
        (normal.x * matrix.m[0][1]) + (normal.y * matrix.m[1][1]) + (normal.z * matrix.m[2][1]),
        (normal.x * matrix.m[0][2]) + (normal.y * matrix.m[1][2]) + (normal.z * matrix.m[2][2]),
    };
}

} // namespace pomdog::math
