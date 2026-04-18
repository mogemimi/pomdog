// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/vector4.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/math_functions.h"
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

Vector4::Vector4(const Vector3& xyz, f32 wIn) noexcept
    : x(xyz.x)
    , y(xyz.y)
    , z(xyz.z)
    , w(wIn)
{
}

Vector4::Vector4(f32 xIn, f32 yIn, f32 zIn, f32 wIn) noexcept
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

Vector4& Vector4::operator*=(f32 factor) noexcept
{
    x *= factor;
    y *= factor;
    z *= factor;
    w *= factor;
    return *this;
}

Vector4& Vector4::operator/=(f32 factor) noexcept
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

Vector4 Vector4::operator*(f32 factor) const noexcept
{
    return Vector4{
        x * factor,
        y * factor,
        z * factor,
        w * factor,
    };
}

Vector4 Vector4::operator/(f32 factor) const noexcept
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

const f32* Vector4::data() const noexcept
{
    return &x;
}

f32* Vector4::data() noexcept
{
    return &x;
}

[[nodiscard]] Vector4
operator*(f32 factor, const Vector4& vector) noexcept
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

[[nodiscard]] f32
length(const Vector4& v) noexcept
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

[[nodiscard]] f32
lengthSquared(const Vector4& v) noexcept
{
    return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
}

[[nodiscard]] f32
distance(const Vector4& a, const Vector4& b) noexcept
{
    return math::length(a - b);
}

[[nodiscard]] f32
distanceSquared(const Vector4& a, const Vector4& b) noexcept
{
    return math::lengthSquared(a - b);
}

[[nodiscard]] f32
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

[[nodiscard]] Vector4
abs(const Vector4& source) noexcept
{
    return Vector4{
        std::abs(source.x),
        std::abs(source.y),
        std::abs(source.z),
        std::abs(source.w),
    };
}

[[nodiscard]] Vector4
floor(const Vector4& source) noexcept
{
    return Vector4{
        std::floor(source.x),
        std::floor(source.y),
        std::floor(source.z),
        std::floor(source.w),
    };
}

[[nodiscard]] Vector4
ceil(const Vector4& source) noexcept
{
    return Vector4{
        std::ceil(source.x),
        std::ceil(source.y),
        std::ceil(source.z),
        std::ceil(source.w),
    };
}

[[nodiscard]] Vector4
round(const Vector4& source) noexcept
{
    return Vector4{
        std::round(source.x),
        std::round(source.y),
        std::round(source.z),
        std::round(source.w),
    };
}

[[nodiscard]] Vector4
min(const Vector4& a, const Vector4& b) noexcept
{
    return Vector4{
        std::min(a.x, b.x),
        std::min(a.y, b.y),
        std::min(a.z, b.z),
        std::min(a.w, b.w),
    };
}

[[nodiscard]] Vector4
max(const Vector4& a, const Vector4& b) noexcept
{
    return Vector4{
        std::max(a.x, b.x),
        std::max(a.y, b.y),
        std::max(a.z, b.z),
        std::max(a.w, b.w),
    };
}

[[nodiscard]] Vector4
saturate(const Vector4& source) noexcept
{
    return Vector4{
        std::clamp(source.x, 0.0f, 1.0f),
        std::clamp(source.y, 0.0f, 1.0f),
        std::clamp(source.z, 0.0f, 1.0f),
        std::clamp(source.w, 0.0f, 1.0f),
    };
}

[[nodiscard]] Vector4
clamp(const Vector4& source, const Vector4& min, const Vector4& max) noexcept
{
    return Vector4{
        math::clamp(source.x, min.x, max.x),
        math::clamp(source.y, min.y, max.y),
        math::clamp(source.z, min.z, max.z),
        math::clamp(source.w, min.w, max.w),
    };
}

[[nodiscard]] Vector4
lerp(const Vector4& source1, const Vector4& source2, f32 amount)
{
    return Vector4{
        math::lerp(source1.x, source2.x, amount),
        math::lerp(source1.y, source2.y, amount),
        math::lerp(source1.z, source2.z, amount),
        math::lerp(source1.w, source2.w, amount),
    };
}

[[nodiscard]] Vector4
smoothstep(const Vector4& source1, const Vector4& source2, f32 amount)
{
    return Vector4{
        math::smoothstep(source1.x, source2.x, amount),
        math::smoothstep(source1.y, source2.y, amount),
        math::smoothstep(source1.z, source2.z, amount),
        math::smoothstep(source1.w, source2.w, amount),
    };
}

} // namespace pomdog::math
