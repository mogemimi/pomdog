// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/vector2.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/math.h"
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

Vector2::Vector2(float x, float y) noexcept
    : X(x)
    , Y(y)
{
}

Vector2& Vector2::operator+=(const Vector2& other) noexcept
{
    this->X += other.X;
    this->Y += other.Y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& other) noexcept
{
    this->X -= other.X;
    this->Y -= other.Y;
    return *this;
}

Vector2& Vector2::operator*=(float factor) noexcept
{
    this->X *= factor;
    this->Y *= factor;
    return *this;
}

Vector2& Vector2::operator/=(float factor) noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    this->X /= factor;
    this->Y /= factor;
    return *this;
}

Vector2 Vector2::operator+() const noexcept
{
    return *this;
}

Vector2 Vector2::operator-() const noexcept
{
    return Vector2(-X, -Y);
}

Vector2 Vector2::operator*(float factor) const noexcept
{
    return Vector2(X * factor, Y * factor);
}

Vector2 Vector2::operator/(float factor) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    return Vector2(X / factor, Y / factor);
}

Vector2 Vector2::operator+(const Vector2& other) const noexcept
{
    return Vector2(X + other.X, Y + other.Y);
}

Vector2 Vector2::operator-(const Vector2& other) const noexcept
{
    return Vector2(X - other.X, Y - other.Y);
}

Vector2 Vector2::operator*(const Vector2& other) const noexcept
{
    return Vector2(X * other.X, Y * other.Y);
}

Vector2 Vector2::operator/(const Vector2& other) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(other.X) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(other.Y) != FP_ZERO);

    return Vector2(X / other.X, Y / other.Y);
}

bool Vector2::operator==(const Vector2& other) const noexcept
{
    return X == other.X && Y == other.Y;
}

bool Vector2::operator!=(const Vector2& other) const noexcept
{
    return X != other.X || Y != other.Y;
}

const float* Vector2::Data() const noexcept
{
    return &X;
}

float* Vector2::Data() noexcept
{
    return &X;
}

Vector2 Vector2::Zero() noexcept
{
    return Vector2{0.0f, 0.0f};
}

[[nodiscard]] Vector2
operator*(float factor, const Vector2& vector) noexcept
{
    return Vector2(factor * vector.X, factor * vector.Y);
}

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] float
Length(const Vector2& v) noexcept
{
    return std::sqrt(v.X * v.X + v.Y * v.Y);
}

[[nodiscard]] float
LengthSquared(const Vector2& v) noexcept
{
    return v.X * v.X + v.Y * v.Y;
}

[[nodiscard]] float
Distance(const Vector2& a, const Vector2& b) noexcept
{
    return math::Length(a - b);
}

[[nodiscard]] float
DistanceSquared(const Vector2& a, const Vector2& b) noexcept
{
    return math::LengthSquared(a - b);
}

[[nodiscard]] float
Dot(const Vector2& a, const Vector2& b) noexcept
{
    return a.X * b.X + a.Y * b.Y;
}

[[nodiscard]] float
Cross(const Vector2& a, const Vector2& b) noexcept
{
    return a.X * b.Y - a.Y * b.X;
}

[[nodiscard]] Vector2
Min(const Vector2& a, const Vector2& b) noexcept
{
    return Vector2{std::min(a.X, b.X), std::min(a.Y, b.Y)};
}

[[nodiscard]] Vector2
Max(const Vector2& a, const Vector2& b) noexcept
{
    return Vector2{std::max(a.X, b.X), std::max(a.Y, b.Y)};
}

[[nodiscard]] Vector2
Clamp(const Vector2& source, const Vector2& min, const Vector2& max) noexcept
{
    return Vector2{
        math::Clamp(source.X, min.X, max.X),
        math::Clamp(source.Y, min.Y, max.Y),
    };
}

[[nodiscard]] Vector2
Lerp(const Vector2& source1, const Vector2& source2, float amount)
{
    return Vector2{
        math::Lerp(source1.X, source2.X, amount),
        math::Lerp(source1.Y, source2.Y, amount),
    };
}

[[nodiscard]] Vector2
SmoothStep(const Vector2& source1, const Vector2& source2, float amount)
{
    return Vector2{
        math::SmoothStep(source1.X, source2.X, amount),
        math::SmoothStep(source1.Y, source2.Y, amount),
    };
}

[[nodiscard]] Vector2
Normalize(const Vector2& source) noexcept
{
    const auto length = math::Length(source);

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
Rotate(const Vector2& vector, const Radian<float>& radian) noexcept
{
    const auto sin = std::sin(radian.value);
    const auto cos = std::cos(radian.value);
    return Vector2{
        (cos * vector.X) - (sin * vector.Y),
        (sin * vector.X) + (cos * vector.Y),
    };
}

[[nodiscard]] Vector2
Transform(const Vector2& position, const Matrix3x2& matrix) noexcept
{
    return Vector2{
        (position.X * matrix.m[0][0]) + (position.Y * matrix.m[1][0]) + matrix.m[2][0],
        (position.X * matrix.m[0][1]) + (position.Y * matrix.m[1][1]) + matrix.m[2][1],
    };
}

[[nodiscard]] Vector2
Transform(const Vector2& position, const Matrix4x4& matrix) noexcept
{
    return Vector2{
        (position.X * matrix.m[0][0]) + (position.Y * matrix.m[1][0]) + matrix.m[3][0],
        (position.X * matrix.m[0][1]) + (position.Y * matrix.m[1][1]) + matrix.m[3][1],
    };
}

[[nodiscard]] Vector2
Transform(const Vector2& position, const Quaternion& rotation) noexcept
{
    const auto x = 2 * (position.Y * -rotation.Z);
    const auto y = 2 * (position.X * rotation.Z);
    const auto z = 2 * (position.Y * rotation.X - position.X * rotation.Y);

    return Vector2{
        position.X + x * rotation.W + (rotation.Y * z - rotation.Z * y),
        position.Y + y * rotation.W + (rotation.Z * x - rotation.X * z),
    };
}

[[nodiscard]] Vector2
TransformNormal(const Vector2& normal, const Matrix4x4& matrix) noexcept
{
    return Vector2{
        (normal.X * matrix.m[0][0]) + (normal.Y * matrix.m[1][0]),
        (normal.X * matrix.m[0][1]) + (normal.Y * matrix.m[1][1]),
    };
}

} // namespace pomdog::math
