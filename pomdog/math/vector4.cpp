// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/vector4.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/math/matrix4x4.hpp"
#include "pomdog/math/vector3.hpp"
#include "pomdog/utility/assert.hpp"

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

Vector4& Vector4::operator*=(float scaleFactor) noexcept
{
    X *= scaleFactor;
    Y *= scaleFactor;
    Z *= scaleFactor;
    W *= scaleFactor;
    return *this;
}

Vector4& Vector4::operator/=(float scaleFactor)
{
    X /= scaleFactor;
    Y /= scaleFactor;
    Z /= scaleFactor;
    W /= scaleFactor;
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

Vector4 Vector4::operator/(const Vector4& other) const
{
    POMDOG_ASSERT(other != Vector4(0, 0, 0, 0));
    return Vector4(
        X / other.X,
        Y / other.Y,
        Z / other.Z,
        W / other.W);
}

Vector4 Vector4::operator*(float scaleFactor) const noexcept
{
    return Vector4(
        X * scaleFactor,
        Y * scaleFactor,
        Z * scaleFactor,
        W * scaleFactor);
}

Vector4 Vector4::operator/(float scaleFactor) const
{
    POMDOG_ASSERT(scaleFactor != 0);
    return Vector4(
        X / scaleFactor,
        Y / scaleFactor,
        Z / scaleFactor,
        W / scaleFactor);
}

bool Vector4::operator==(const Vector4& other) const noexcept
{
    return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
}

bool Vector4::operator!=(const Vector4& other) const noexcept
{
    return X != other.X || Y != other.Y || Z != other.Z || W != other.W;
}

float Vector4::Length() const noexcept
{
    return std::sqrt(X * X + Y * Y + Z * Z + W * W);
}

float Vector4::LengthSquared() const noexcept
{
    return X * X + Y * Y + Z * Z + W * W;
}

float Vector4::Distance(const Vector4& a, const Vector4& b) noexcept
{
    return (a - b).Length();
}

float Vector4::DistanceSquared(const Vector4& a, const Vector4& b) noexcept
{
    return (a - b).LengthSquared();
}

float Vector4::Dot(const Vector4& a, const Vector4& b) noexcept
{
    return a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W;
}

void Vector4::Normalize() noexcept
{
    *this = Normalize(*this);
}

Vector4
Vector4::Normalize(const Vector4& source) noexcept
{
    auto result = source;
    Normalize(source, result);
    return result;
}

void Vector4::Normalize(const Vector4& source, Vector4& result) noexcept
{
    const auto length = source.Length();

    if (length > std::numeric_limits<decltype(length)>::epsilon()) {
        constexpr float One = 1;
        const auto inverseLength = One / length;
        result.X = source.X * inverseLength;
        result.Y = source.Y * inverseLength;
        result.Z = source.Z * inverseLength;
        result.W = source.W * inverseLength;
    }
}

Vector4
Vector4::Transform(const Vector4& vector, const Matrix4x4& matrix) noexcept
{
    return Vector4{
        (vector.X * matrix.m[0][0]) + (vector.Y * matrix.m[1][0]) + (vector.Z * matrix.m[2][0]) + (vector.W * matrix.m[3][0]),
        (vector.X * matrix.m[0][1]) + (vector.Y * matrix.m[1][1]) + (vector.Z * matrix.m[2][1]) + (vector.W * matrix.m[3][1]),
        (vector.X * matrix.m[0][2]) + (vector.Y * matrix.m[1][2]) + (vector.Z * matrix.m[2][2]) + (vector.W * matrix.m[3][2]),
        (vector.X * matrix.m[0][3]) + (vector.Y * matrix.m[1][3]) + (vector.Z * matrix.m[2][3]) + (vector.W * matrix.m[3][3]),
    };
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
operator*(float scaleFactor, const Vector4& vector) noexcept
{
    return Vector4(
        scaleFactor * vector.X,
        scaleFactor * vector.Y,
        scaleFactor * vector.Z,
        scaleFactor * vector.W);
}

} // namespace pomdog
