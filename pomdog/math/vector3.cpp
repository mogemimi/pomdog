// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/vector3.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/math/math.hpp"
#include "pomdog/math/matrix4x4.hpp"
#include "pomdog/math/quaternion.hpp"
#include "pomdog/math/vector2.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Vector3 const Vector3::Zero = {0, 0, 0};

Vector3 const Vector3::UnitX = {1, 0, 0};

Vector3 const Vector3::UnitY = {0, 1, 0};

Vector3 const Vector3::UnitZ = {0, 0, 1};

Vector3::Vector3() noexcept = default;

Vector3::Vector3(float x, float y, float z) noexcept
    : X(x)
    , Y(y)
    , Z(z)
{
}

Vector3::Vector3(const Vector2& vec, float z) noexcept
    : X(vec.X)
    , Y(vec.Y)
    , Z(z)
{
}

Vector3& Vector3::operator+=(const Vector3& vec) noexcept
{
    X += vec.X;
    Y += vec.Y;
    Z += vec.Z;
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& vec) noexcept
{
    X -= vec.X;
    Y -= vec.Y;
    Z -= vec.Z;
    return *this;
}

Vector3& Vector3::operator*=(float scaleFactor) noexcept
{
    X *= scaleFactor;
    Y *= scaleFactor;
    Z *= scaleFactor;
    return *this;
}

Vector3& Vector3::operator/=(float scaleFactor) noexcept
{
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_INFINITE);
    X /= scaleFactor;
    Y /= scaleFactor;
    Z /= scaleFactor;
    return *this;
}

Vector3 Vector3::operator+() const noexcept
{
    return *this;
}

Vector3 Vector3::operator-() const noexcept
{
    return Vector3(-X, -Y, -Z);
}

Vector3 Vector3::operator+(const Vector3& other) const noexcept
{
    return Vector3(
        X + other.X,
        Y + other.Y,
        Z + other.Z);
}

Vector3 Vector3::operator-(const Vector3& other) const noexcept
{
    return Vector3(
        X - other.X,
        Y - other.Y,
        Z - other.Z);
}

Vector3 Vector3::operator*(const Vector3& other) const noexcept
{
    return Vector3(
        X * other.X,
        Y * other.Y,
        Z * other.Z);
}

Vector3 Vector3::operator/(const Vector3& other) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(other.X) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(other.Y) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(other.Z) != FP_ZERO);

    return Vector3(
        X / other.X,
        Y / other.Y,
        Z / other.Z);
}

Vector3 Vector3::operator*(float scaleFactor) const noexcept
{
    return Vector3(
        X * scaleFactor,
        Y * scaleFactor,
        Z * scaleFactor);
}

Vector3 Vector3::operator/(float scaleFactor) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_INFINITE);
    return Vector3(
        X / scaleFactor,
        Y / scaleFactor,
        Z / scaleFactor);
}

bool Vector3::operator==(const Vector3& other) const noexcept
{
    return X == other.X && Y == other.Y && Z == other.Z;
}

bool Vector3::operator!=(const Vector3& other) const noexcept
{
    return X != other.X || Y != other.Y || Z != other.Z;
}

float Vector3::Length() const noexcept
{
    return std::sqrt(X * X + Y * Y + Z * Z);
}

float Vector3::LengthSquared() const noexcept
{
    return X * X + Y * Y + Z * Z;
}

float Vector3::Distance(const Vector3& a, const Vector3& b) noexcept
{
    return (a - b).Length();
}

float Vector3::DistanceSquared(const Vector3& a, const Vector3& b) noexcept
{
    return (a - b).LengthSquared();
}

float Vector3::Dot(const Vector3& a, const Vector3& b) noexcept
{
    return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
}

Vector3
Vector3::Cross(const Vector3& a, const Vector3& b) noexcept
{
    return Vector3(
        a.Y * b.Z - a.Z * b.Y,
        a.Z * b.X - a.X * b.Z,
        a.X * b.Y - a.Y * b.X);
}

Vector3
Vector3::Min(const Vector3& a, const Vector3& b) noexcept
{
    return Vector3(
        std::min(a.X, b.X),
        std::min(a.Y, b.Y),
        std::min(a.Z, b.Z));
}

Vector3
Vector3::Max(const Vector3& a, const Vector3& b) noexcept
{
    return Vector3(
        std::max(a.X, b.X),
        std::max(a.Y, b.Y),
        std::max(a.Z, b.Z));
}

Vector3
Vector3::Clamp(const Vector3& source, const Vector3& min, const Vector3& max) noexcept
{
    return Vector3(
        math::Clamp(source.X, min.X, max.X),
        math::Clamp(source.Y, min.Y, max.Y),
        math::Clamp(source.Z, min.Z, max.Z));
}

Vector3
Vector3::Lerp(const Vector3& source1, const Vector3& source2, float amount)
{
    return Vector3(
        math::Lerp(source1.X, source2.X, amount),
        math::Lerp(source1.Y, source2.Y, amount),
        math::Lerp(source1.Z, source2.Z, amount));
}

Vector3
Vector3::SmoothStep(const Vector3& source1, const Vector3& source2, float amount)
{
    return Vector3(
        math::SmoothStep(source1.X, source2.X, amount),
        math::SmoothStep(source1.Y, source2.Y, amount),
        math::SmoothStep(source1.Z, source2.Z, amount));
}

void Vector3::Normalize() noexcept
{
    *this = Normalize(*this);
}

Vector3
Vector3::Normalize(const Vector3& source) noexcept
{
    auto result = source;
    Normalize(source, result);
    return result;
}

void Vector3::Normalize(const Vector3& source, Vector3& result) noexcept
{
    const auto length = source.Length();

    if (length > std::numeric_limits<decltype(length)>::epsilon()) {
        constexpr float One = 1;
        const auto inverseLength = One / length;
        result.X = source.X * inverseLength;
        result.Y = source.Y * inverseLength;
        result.Z = source.Z * inverseLength;
    }
}

Vector3
Vector3::Transform(const Vector3& position, const Matrix4x4& matrix) noexcept
{
    return Vector3(
        (position.X * matrix.m[0][0]) + (position.Y * matrix.m[1][0]) + (position.Z * matrix.m[2][0]) + matrix.m[3][0],
        (position.X * matrix.m[0][1]) + (position.Y * matrix.m[1][1]) + (position.Z * matrix.m[2][1]) + matrix.m[3][1],
        (position.X * matrix.m[0][2]) + (position.Y * matrix.m[1][2]) + (position.Z * matrix.m[2][2]) + matrix.m[3][2]);
}

Vector3
Vector3::TransformNormal(const Vector3& normal, const Matrix4x4& matrix) noexcept
{
    return Vector3(
        (normal.X * matrix.m[0][0]) + (normal.Y * matrix.m[1][0]) + (normal.Z * matrix.m[2][0]),
        (normal.X * matrix.m[0][1]) + (normal.Y * matrix.m[1][1]) + (normal.Z * matrix.m[2][1]),
        (normal.X * matrix.m[0][2]) + (normal.Y * matrix.m[1][2]) + (normal.Z * matrix.m[2][2]));
}

Vector3
Vector3::ToEulerAngles(const Quaternion& q)
{
    constexpr float Two = 2;
    const auto xx = q.X * q.X;
    const auto yy = q.Y * q.Y;
    const auto zz = q.Z * q.Z;
    const auto ww = q.W * q.W;
    auto roll = std::atan2(Two * (q.W * q.Z + q.X * q.Y), ww + xx - yy - zz);
    auto yaw = std::asin(Two * (q.W * q.Y - q.Z * q.X));
    auto pitch = std::atan2(Two * (q.W * q.X + q.Y * q.Z), ww - xx - yy + zz);
    return Vector3(pitch, yaw, roll);
}

const float* Vector3::Data() const noexcept
{
    return &X;
}

float* Vector3::Data() noexcept
{
    return &X;
}

[[nodiscard]] Vector3
operator*(float scaleFactor, const Vector3& vector) noexcept
{
    return Vector3(
        scaleFactor * vector.X,
        scaleFactor * vector.Y,
        scaleFactor * vector.Z);
}

} // namespace pomdog
