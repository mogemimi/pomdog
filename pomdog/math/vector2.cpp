// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/vector2.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/math/math.hpp"
#include "pomdog/math/matrix3x2.hpp"
#include "pomdog/math/matrix4x4.hpp"
#include "pomdog/math/quaternion.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Vector2 const Vector2::Zero = {0, 0};

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

Vector2& Vector2::operator*=(float scaleFactor) noexcept
{
    this->X *= scaleFactor;
    this->Y *= scaleFactor;
    return *this;
}

Vector2& Vector2::operator/=(float scaleFactor)
{
    this->X /= scaleFactor;
    this->Y /= scaleFactor;
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

Vector2 Vector2::operator*(float scaleFactor) const noexcept
{
    return Vector2(X * scaleFactor, Y * scaleFactor);
}

Vector2 Vector2::operator/(float scaleFactor) const
{
    POMDOG_ASSERT(scaleFactor != 0);
    return Vector2(X / scaleFactor, Y / scaleFactor);
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

Vector2 Vector2::operator/(const Vector2& other) const
{
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

float Vector2::Length() const noexcept
{
    return std::sqrt(X * X + Y * Y);
}

float Vector2::LengthSquared() const noexcept
{
    return X * X + Y * Y;
}

float Vector2::Distance(const Vector2& a, const Vector2& b) noexcept
{
    return (a - b).Length();
}

float Vector2::DistanceSquared(const Vector2& a, const Vector2& b) noexcept
{
    return (a - b).LengthSquared();
}

float Vector2::Dot(const Vector2& a, const Vector2& b) noexcept
{
    return a.X * b.X + a.Y * b.Y;
}

float Vector2::Cross(const Vector2& a, const Vector2& b) noexcept
{
    return a.X * b.Y - a.Y * b.X;
}

Vector2
Vector2::Min(const Vector2& a, const Vector2& b) noexcept
{
    return Vector2{std::min(a.X, b.X), std::min(a.Y, b.Y)};
}

Vector2
Vector2::Max(const Vector2& a, const Vector2& b) noexcept
{
    return Vector2{std::max(a.X, b.X), std::max(a.Y, b.Y)};
}

Vector2
Vector2::Clamp(const Vector2& source, const Vector2& min, const Vector2& max) noexcept
{
    return Vector2{
        math::Clamp(source.X, min.X, max.X),
        math::Clamp(source.Y, min.Y, max.Y)};
}

Vector2
Vector2::Lerp(const Vector2& source1, const Vector2& source2, float amount)
{
    return Vector2(
        math::Lerp(source1.X, source2.X, amount),
        math::Lerp(source1.Y, source2.Y, amount));
}

Vector2
Vector2::SmoothStep(const Vector2& source1, const Vector2& source2, float amount)
{
    return Vector2(
        math::SmoothStep(source1.X, source2.X, amount),
        math::SmoothStep(source1.Y, source2.Y, amount));
}

void Vector2::Normalize() noexcept
{
    *this = Normalize(*this);
}

Vector2
Vector2::Normalize(const Vector2& source) noexcept
{
    auto result = source;
    Normalize(source, result);
    return result;
}

void Vector2::Normalize(const Vector2& source, Vector2& result) noexcept
{
    const auto length = source.Length();

    if (length > std::numeric_limits<decltype(length)>::epsilon()) {
        constexpr float One = 1;
        const auto InverseLength = One / length;
        result.X = source.X * InverseLength;
        result.Y = source.Y * InverseLength;
        return;
    }
}

Vector2
Vector2::Rotate(const Vector2& vector, const Radian<float>& radian) noexcept
{
    const auto sin = std::sin(radian.value);
    const auto cos = std::cos(radian.value);
    return Vector2{
        (cos * vector.X) - (sin * vector.Y),
        (sin * vector.X) + (cos * vector.Y)};
}

Vector2
Vector2::Transform(const Vector2& position, const Matrix3x2& matrix) noexcept
{
    return Vector2(
        (position.X * matrix.m[0][0]) + (position.Y * matrix.m[1][0]) + matrix.m[2][0],
        (position.X * matrix.m[0][1]) + (position.Y * matrix.m[1][1]) + matrix.m[2][1]);
}

Vector2
Vector2::Transform(const Vector2& position, const Matrix4x4& matrix) noexcept
{
    return Vector2(
        (position.X * matrix.m[0][0]) + (position.Y * matrix.m[1][0]) + matrix.m[3][0],
        (position.X * matrix.m[0][1]) + (position.Y * matrix.m[1][1]) + matrix.m[3][1]);
}

Vector2
Vector2::Transform(const Vector2& position, const Quaternion& rotation) noexcept
{
    const auto x = 2 * (position.Y * -rotation.Z);
    const auto y = 2 * (position.X * rotation.Z);
    const auto z = 2 * (position.Y * rotation.X - position.X * rotation.Y);

    return Vector2(
        position.X + x * rotation.W + (rotation.Y * z - rotation.Z * y),
        position.Y + y * rotation.W + (rotation.Z * x - rotation.X * z));
}

Vector2
Vector2::TransformNormal(const Vector2& normal, const Matrix4x4& matrix) noexcept
{
    return Vector2(
        (normal.X * matrix.m[0][0]) + (normal.Y * matrix.m[1][0]),
        (normal.X * matrix.m[0][1]) + (normal.Y * matrix.m[1][1]));
}

const float* Vector2::Data() const noexcept
{
    return &X;
}

float* Vector2::Data() noexcept
{
    return &X;
}

[[nodiscard]] Vector2
operator*(float scaleFactor, const Vector2& vector) noexcept
{
    return Vector2(scaleFactor * vector.X, scaleFactor * vector.Y);
}

} // namespace pomdog
