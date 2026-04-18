// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/vector2.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/math/matrix3x2.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/quaternion.h"
#include "pomdog/math/radian.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Vector2::Vector2() noexcept = default;

Vector2::Vector2(f32 xIn, f32 yIn) noexcept
    : x(xIn)
    , y(yIn)
{
}

Vector2& Vector2::operator+=(const Vector2& other) noexcept
{
    x += other.x;
    y += other.y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& other) noexcept
{
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector2& Vector2::operator*=(f32 factor) noexcept
{
    x *= factor;
    y *= factor;
    return *this;
}

Vector2& Vector2::operator/=(f32 factor) noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    x /= factor;
    y /= factor;
    return *this;
}

Vector2 Vector2::operator+() const noexcept
{
    return *this;
}

Vector2 Vector2::operator-() const noexcept
{
    return Vector2{-x, -y};
}

Vector2 Vector2::operator*(f32 factor) const noexcept
{
    return Vector2{x * factor, y * factor};
}

Vector2 Vector2::operator/(f32 factor) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    return Vector2{x / factor, y / factor};
}

Vector2 Vector2::operator+(const Vector2& other) const noexcept
{
    return Vector2{x + other.x, y + other.y};
}

Vector2 Vector2::operator-(const Vector2& other) const noexcept
{
    return Vector2{x - other.x, y - other.y};
}

Vector2 Vector2::operator*(const Vector2& other) const noexcept
{
    return Vector2{x * other.x, y * other.y};
}

Vector2 Vector2::operator/(const Vector2& other) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(other.x) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(other.y) != FP_ZERO);

    return Vector2{x / other.x, y / other.y};
}

bool Vector2::operator==(const Vector2& other) const noexcept
{
    return x == other.x && y == other.y;
}

bool Vector2::operator!=(const Vector2& other) const noexcept
{
    return x != other.x || y != other.y;
}

const f32* Vector2::data() const noexcept
{
    return &x;
}

f32* Vector2::data() noexcept
{
    return &x;
}

Vector2 Vector2::createZero() noexcept
{
    return Vector2{0.0f, 0.0f};
}

[[nodiscard]] Vector2
operator*(f32 factor, const Vector2& vector) noexcept
{
    return Vector2{factor * vector.x, factor * vector.y};
}

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] f32
length(const Vector2& v) noexcept
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

[[nodiscard]] f32
lengthSquared(const Vector2& v) noexcept
{
    return v.x * v.x + v.y * v.y;
}

[[nodiscard]] f32
distance(const Vector2& a, const Vector2& b) noexcept
{
    return math::length(a - b);
}

[[nodiscard]] f32
distanceSquared(const Vector2& a, const Vector2& b) noexcept
{
    return math::lengthSquared(a - b);
}

[[nodiscard]] f32
dot(const Vector2& a, const Vector2& b) noexcept
{
    return a.x * b.x + a.y * b.y;
}

[[nodiscard]] f32
cross(const Vector2& a, const Vector2& b) noexcept
{
    return a.x * b.y - a.y * b.x;
}

[[nodiscard]] Vector2
min(const Vector2& a, const Vector2& b) noexcept
{
    return Vector2{std::min(a.x, b.x), std::min(a.y, b.y)};
}

[[nodiscard]] Vector2
max(const Vector2& a, const Vector2& b) noexcept
{
    return Vector2{std::max(a.x, b.x), std::max(a.y, b.y)};
}

[[nodiscard]] Vector2
clamp(const Vector2& source, const Vector2& min, const Vector2& max) noexcept
{
    return Vector2{
        math::clamp(source.x, min.x, max.x),
        math::clamp(source.y, min.y, max.y),
    };
}

[[nodiscard]] Vector2
lerp(const Vector2& source1, const Vector2& source2, f32 amount)
{
    return Vector2{
        math::lerp(source1.x, source2.x, amount),
        math::lerp(source1.y, source2.y, amount),
    };
}

[[nodiscard]] Vector2
smoothstep(const Vector2& source1, const Vector2& source2, f32 amount)
{
    return Vector2{
        math::smoothstep(source1.x, source2.x, amount),
        math::smoothstep(source1.y, source2.y, amount),
    };
}

[[nodiscard]] Vector2
normalize(const Vector2& source) noexcept
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

[[nodiscard]] Vector2
rotate(const Vector2& vector, const Radian<f32>& radian) noexcept
{
    const auto sin = std::sin(radian.get());
    const auto cos = std::cos(radian.get());
    return Vector2{
        (cos * vector.x) - (sin * vector.y),
        (sin * vector.x) + (cos * vector.y),
    };
}

[[nodiscard]] Vector2
transform(const Vector2& position, const Matrix3x2& matrix) noexcept
{
    return Vector2{
        (position.x * matrix.m[0][0]) + (position.y * matrix.m[1][0]) + matrix.m[2][0],
        (position.x * matrix.m[0][1]) + (position.y * matrix.m[1][1]) + matrix.m[2][1],
    };
}

[[nodiscard]] Vector2
transform(const Vector2& position, const Matrix4x4& matrix) noexcept
{
    return Vector2{
        (position.x * matrix.m[0][0]) + (position.y * matrix.m[1][0]) + matrix.m[3][0],
        (position.x * matrix.m[0][1]) + (position.y * matrix.m[1][1]) + matrix.m[3][1],
    };
}

[[nodiscard]] Vector2
transform(const Vector2& position, const Quaternion& rotation) noexcept
{
    const auto x = 2 * (position.y * -rotation.z);
    const auto y = 2 * (position.x * rotation.z);
    const auto z = 2 * (position.y * rotation.x - position.x * rotation.y);

    return Vector2{
        position.x + x * rotation.w + (rotation.y * z - rotation.z * y),
        position.y + y * rotation.w + (rotation.z * x - rotation.x * z),
    };
}

[[nodiscard]] Vector2
transformNormal(const Vector2& normal, const Matrix4x4& matrix) noexcept
{
    return Vector2{
        (normal.x * matrix.m[0][0]) + (normal.y * matrix.m[1][0]),
        (normal.x * matrix.m[0][1]) + (normal.y * matrix.m[1][1]),
    };
}

[[nodiscard]] Vector2
abs(const Vector2& source) noexcept
{
    return Vector2{
        std::abs(source.x),
        std::abs(source.y),
    };
}

[[nodiscard]] Vector2
floor(const Vector2& source) noexcept
{
    return Vector2{
        std::floor(source.x),
        std::floor(source.y),
    };
}

[[nodiscard]] Vector2
ceil(const Vector2& source) noexcept
{
    return Vector2{
        std::ceil(source.x),
        std::ceil(source.y),
    };
}

[[nodiscard]] Vector2
round(const Vector2& source) noexcept
{
    return Vector2{
        std::round(source.x),
        std::round(source.y),
    };
}

[[nodiscard]] Vector2
saturate(const Vector2& source) noexcept
{
    return Vector2{
        std::clamp(source.x, 0.0f, 1.0f),
        std::clamp(source.y, 0.0f, 1.0f),
    };
}

[[nodiscard]] f32
toPositiveAngle(const Vector2& source) noexcept
{
    const auto angle = std::atan2(source.y, source.x);
    if (angle < 0.0f) {
        return angle + math::TwoPi<f32>;
    }
    return angle;
}

[[nodiscard]] f32
toSignedAngle(const Vector2& source) noexcept
{
    return std::atan2(source.y, source.x);
}

[[nodiscard]] bool
approxEqual(const Vector2& a, const Vector2& b) noexcept
{
    return approxEqual(a.x, b.x) &&
           approxEqual(a.y, b.y);
}

[[nodiscard]] bool
approxEqual(const Vector2& a, const Vector2& b, f32 tolerance) noexcept
{
    return approxEqual(a.x, b.x, tolerance) &&
           approxEqual(a.y, b.y, tolerance);
}

} // namespace pomdog::math
