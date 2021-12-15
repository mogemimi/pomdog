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

Vector3& Vector3::operator*=(float factor) noexcept
{
    X *= factor;
    Y *= factor;
    Z *= factor;
    return *this;
}

Vector3& Vector3::operator/=(float factor) noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    X /= factor;
    Y /= factor;
    Z /= factor;
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

Vector3 Vector3::operator*(float factor) const noexcept
{
    return Vector3(
        X * factor,
        Y * factor,
        Z * factor);
}

Vector3 Vector3::operator/(float factor) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    return Vector3(
        X / factor,
        Y / factor,
        Z / factor);
}

bool Vector3::operator==(const Vector3& other) const noexcept
{
    return X == other.X && Y == other.Y && Z == other.Z;
}

bool Vector3::operator!=(const Vector3& other) const noexcept
{
    return X != other.X || Y != other.Y || Z != other.Z;
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

Vector3 Vector3::Zero() noexcept
{
    return Vector3{0.0f, 0.0f, 0.0f};
}

Vector3 Vector3::UnitX() noexcept
{
    return Vector3{1.0f, 0.0f, 0.0f};
}

Vector3 Vector3::UnitY() noexcept
{
    return Vector3{0.0f, 1.0f, 0.0f};
}

Vector3 Vector3::UnitZ() noexcept
{
    return Vector3{0.0f, 0.0f, 1.0f};
}

[[nodiscard]] Vector3
operator*(float factor, const Vector3& vector) noexcept
{
    return Vector3(
        factor * vector.X,
        factor * vector.Y,
        factor * vector.Z);
}

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] float
Length(const Vector3& v) noexcept
{
    return std::sqrt(v.X * v.X + v.Y * v.Y + v.Z * v.Z);
}

[[nodiscard]] float
LengthSquared(const Vector3& v) noexcept
{
    return v.X * v.X + v.Y * v.Y + v.Z * v.Z;
}

[[nodiscard]] float
Distance(const Vector3& a, const Vector3& b) noexcept
{
    return math::Length(a - b);
}

[[nodiscard]] float
DistanceSquared(const Vector3& a, const Vector3& b) noexcept
{
    return math::LengthSquared(a - b);
}

[[nodiscard]] float
Dot(const Vector3& a, const Vector3& b) noexcept
{
    return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
}

[[nodiscard]] Vector3
Cross(const Vector3& a, const Vector3& b) noexcept
{
    return Vector3{
        a.Y * b.Z - a.Z * b.Y,
        a.Z * b.X - a.X * b.Z,
        a.X * b.Y - a.Y * b.X,
    };
}

[[nodiscard]] Vector3
Min(const Vector3& a, const Vector3& b) noexcept
{
    return Vector3{
        std::min(a.X, b.X),
        std::min(a.Y, b.Y),
        std::min(a.Z, b.Z),
    };
}

[[nodiscard]] Vector3
Max(const Vector3& a, const Vector3& b) noexcept
{
    return Vector3{
        std::max(a.X, b.X),
        std::max(a.Y, b.Y),
        std::max(a.Z, b.Z),
    };
}

[[nodiscard]] Vector3
Clamp(const Vector3& source, const Vector3& min, const Vector3& max) noexcept
{
    return Vector3{
        math::Clamp(source.X, min.X, max.X),
        math::Clamp(source.Y, min.Y, max.Y),
        math::Clamp(source.Z, min.Z, max.Z),
    };
}

[[nodiscard]] Vector3
Lerp(const Vector3& source1, const Vector3& source2, float amount)
{
    return Vector3{
        math::Lerp(source1.X, source2.X, amount),
        math::Lerp(source1.Y, source2.Y, amount),
        math::Lerp(source1.Z, source2.Z, amount),
    };
}

[[nodiscard]] Vector3
SmoothStep(const Vector3& source1, const Vector3& source2, float amount)
{
    return Vector3{
        math::SmoothStep(source1.X, source2.X, amount),
        math::SmoothStep(source1.Y, source2.Y, amount),
        math::SmoothStep(source1.Z, source2.Z, amount),
    };
}

[[nodiscard]] Vector3
Normalize(const Vector3& source) noexcept
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

[[nodiscard]] Vector3
Transform(const Vector3& position, const Matrix4x4& matrix) noexcept
{
    return Vector3{
        (position.X * matrix.m[0][0]) + (position.Y * matrix.m[1][0]) + (position.Z * matrix.m[2][0]) + matrix.m[3][0],
        (position.X * matrix.m[0][1]) + (position.Y * matrix.m[1][1]) + (position.Z * matrix.m[2][1]) + matrix.m[3][1],
        (position.X * matrix.m[0][2]) + (position.Y * matrix.m[1][2]) + (position.Z * matrix.m[2][2]) + matrix.m[3][2],
    };
}

[[nodiscard]] Vector3
TransformNormal(const Vector3& normal, const Matrix4x4& matrix) noexcept
{
    return Vector3{
        (normal.X * matrix.m[0][0]) + (normal.Y * matrix.m[1][0]) + (normal.Z * matrix.m[2][0]),
        (normal.X * matrix.m[0][1]) + (normal.Y * matrix.m[1][1]) + (normal.Z * matrix.m[2][1]),
        (normal.X * matrix.m[0][2]) + (normal.Y * matrix.m[1][2]) + (normal.Z * matrix.m[2][2]),
    };
}

} // namespace pomdog::math
