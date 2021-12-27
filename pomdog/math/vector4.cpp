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

Vector4::Vector4(const Vector3& xyz, float w) noexcept
    : X(xyz.X)
    , Y(xyz.Y)
    , Z(xyz.Z)
    , W(w)
{
}

Vector4::Vector4(float x, float y, float z, float w) noexcept
    : X(x)
    , Y(y)
    , Z(z)
    , W(w)
{
}

Vector4& Vector4::operator+=(const Vector4& other) noexcept
{
    X += other.X;
    Y += other.Y;
    Z += other.Z;
    W += other.W;
    return *this;
}

Vector4& Vector4::operator-=(const Vector4& other) noexcept
{
    X -= other.X;
    Y -= other.Y;
    Z -= other.Z;
    W -= other.W;
    return *this;
}

Vector4& Vector4::operator*=(float factor) noexcept
{
    X *= factor;
    Y *= factor;
    Z *= factor;
    W *= factor;
    return *this;
}

Vector4& Vector4::operator/=(float factor) noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    X /= factor;
    Y /= factor;
    Z /= factor;
    W /= factor;
    return *this;
}

Vector4 Vector4::operator+() const noexcept
{
    return *this;
}

Vector4 Vector4::operator-() const noexcept
{
    return Vector4(-X, -Y, -Z, -W);
}

Vector4 Vector4::operator+(const Vector4& other) const noexcept
{
    return Vector4(
        X + other.X,
        Y + other.Y,
        Z + other.Z,
        W + other.W);
}

Vector4 Vector4::operator-(const Vector4& other) const noexcept
{
    return Vector4(
        X - other.X,
        Y - other.Y,
        Z - other.Z,
        W - other.W);
}

Vector4 Vector4::operator*(const Vector4& other) const noexcept
{
    return Vector4(
        X * other.X,
        Y * other.Y,
        Z * other.Z,
        W * other.W);
}

Vector4 Vector4::operator/(const Vector4& other) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(other.X) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(other.Y) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(other.Z) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(other.W) != FP_ZERO);
    return Vector4(
        X / other.X,
        Y / other.Y,
        Z / other.Z,
        W / other.W);
}

Vector4 Vector4::operator*(float factor) const noexcept
{
    return Vector4(
        X * factor,
        Y * factor,
        Z * factor,
        W * factor);
}

Vector4 Vector4::operator/(float factor) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    return Vector4(
        X / factor,
        Y / factor,
        Z / factor,
        W / factor);
}

bool Vector4::operator==(const Vector4& other) const noexcept
{
    return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
}

bool Vector4::operator!=(const Vector4& other) const noexcept
{
    return X != other.X || Y != other.Y || Z != other.Z || W != other.W;
}

const float* Vector4::Data() const noexcept
{
    return &X;
}

float* Vector4::Data() noexcept
{
    return &X;
}

[[nodiscard]] Vector4
operator*(float factor, const Vector4& vector) noexcept
{
    return Vector4(
        factor * vector.X,
        factor * vector.Y,
        factor * vector.Z,
        factor * vector.W);
}

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] float
Length(const Vector4& v) noexcept
{
    return std::sqrt(v.X * v.X + v.Y * v.Y + v.Z * v.Z + v.W * v.W);
}

[[nodiscard]] float
LengthSquared(const Vector4& v) noexcept
{
    return v.X * v.X + v.Y * v.Y + v.Z * v.Z + v.W * v.W;
}

[[nodiscard]] float
Distance(const Vector4& a, const Vector4& b) noexcept
{
    return math::Length(a - b);
}

[[nodiscard]] float
DistanceSquared(const Vector4& a, const Vector4& b) noexcept
{
    return math::LengthSquared(a - b);
}

[[nodiscard]] float
Dot(const Vector4& a, const Vector4& b) noexcept
{
    return a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W;
}

[[nodiscard]] Vector4
Normalize(const Vector4& source) noexcept
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

[[nodiscard]] Vector4
Transform(const Vector4& vector, const Matrix4x4& matrix) noexcept
{
    return Vector4{
        (vector.X * matrix.m[0][0]) + (vector.Y * matrix.m[1][0]) + (vector.Z * matrix.m[2][0]) + (vector.W * matrix.m[3][0]),
        (vector.X * matrix.m[0][1]) + (vector.Y * matrix.m[1][1]) + (vector.Z * matrix.m[2][1]) + (vector.W * matrix.m[3][1]),
        (vector.X * matrix.m[0][2]) + (vector.Y * matrix.m[1][2]) + (vector.Z * matrix.m[2][2]) + (vector.W * matrix.m[3][2]),
        (vector.X * matrix.m[0][3]) + (vector.Y * matrix.m[1][3]) + (vector.Z * matrix.m[2][3]) + (vector.W * matrix.m[3][3]),
    };
}

} // namespace pomdog::math
