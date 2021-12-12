// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/quaternion.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/math/math.hpp"
#include "pomdog/math/matrix3x3.hpp"
#include "pomdog/math/matrix4x4.hpp"
#include "pomdog/math/radian.hpp"
#include "pomdog/math/vector3.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cmath>
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Quaternion const Quaternion::Identity = {0, 0, 0, 1};

Quaternion::Quaternion() noexcept = default;

Quaternion::Quaternion(float x, float y, float z, float w) noexcept
    : X(x)
    , Y(y)
    , Z(z)
    , W(w)
{
}

Quaternion& Quaternion::operator+=(const Quaternion& other) noexcept
{
    X += other.X;
    Y += other.Y;
    Z += other.Z;
    W += other.W;
    return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion& other) noexcept
{
    X -= other.X;
    Y -= other.Y;
    Z -= other.Z;
    W -= other.W;
    return *this;
}

Quaternion& Quaternion::operator*=(const Quaternion& other) noexcept
{
    return (*this) = (*this) * other;
}

Quaternion& Quaternion::operator*=(float scaleFactor) noexcept
{
    X *= scaleFactor;
    Y *= scaleFactor;
    Z *= scaleFactor;
    W *= scaleFactor;
    return *this;
}

Quaternion& Quaternion::operator/=(float scaleFactor)
{
    POMDOG_ASSERT(scaleFactor != 0);
    X /= scaleFactor;
    W /= scaleFactor;
    Z /= scaleFactor;
    W /= scaleFactor;
    return *this;
}

Quaternion Quaternion::operator+() const noexcept
{
    return *this;
}

Quaternion Quaternion::operator-() const noexcept
{
    return Quaternion(-X, -Y, -Z, -W);
}

Quaternion Quaternion::operator+(const Quaternion& other) const noexcept
{
    return Quaternion(
        X + other.X,
        Y + other.Y,
        Z + other.Z,
        W + other.W);
}

Quaternion Quaternion::operator-(const Quaternion& other) const noexcept
{
    return Quaternion(
        X - other.X,
        Y - other.Y,
        Z - other.Z,
        W - other.W);
}

Quaternion Quaternion::operator*(const Quaternion& other) const noexcept
{
    // Quaternion(
    //     w * other.Xyz + xyz * other.W + Vector3::Cross(this->xyz, other.Xyz),
    //     w * other.W - Vector3::Dot(this->xyz, other.Xyz));
    return Quaternion(
        W * other.X + X * other.W + Y * other.Z - Z * other.Y,
        W * other.Y + Y * other.W + Z * other.X - X * other.Z,
        W * other.Z + Z * other.W + X * other.Y - Y * other.X,
        W * other.W - X * other.X - Y * other.Y - Z * other.Z);
}

Quaternion Quaternion::operator*(float scaleFactor) const noexcept
{
    return Quaternion(
        X * scaleFactor,
        Y * scaleFactor,
        Z * scaleFactor,
        W * scaleFactor);
}

Quaternion Quaternion::operator/(float scaleFactor) const
{
    POMDOG_ASSERT(scaleFactor != 0);
    return Quaternion(
        X / scaleFactor,
        Y / scaleFactor,
        Z / scaleFactor,
        W / scaleFactor);
}

bool Quaternion::operator==(const Quaternion& other) const noexcept
{
    return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
}

bool Quaternion::operator!=(const Quaternion& other) const noexcept
{
    return X != other.X || Y != other.Y || Z != other.Z || W != other.W;
}

float Quaternion::Length() const noexcept
{
    return std::sqrt(X * X + Y * Y + Z * Z + W * W);
}

float Quaternion::LengthSquared() const noexcept
{
    return X * X + Y * Y + Z * Z + W * W;
}

float Quaternion::Dot(const Quaternion& a, const Quaternion& b) noexcept
{
    return a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W;
}

void Quaternion::Normalize() noexcept
{
    *this = Normalize(*this);
}

void Quaternion::Normalize(const Quaternion& source, Quaternion& result) noexcept
{
    const auto length = source.Length();

    if (length > std::numeric_limits<decltype(length)>::epsilon()) {
        const auto InverseLength = 1 / length;
        result.X = source.X * InverseLength;
        result.Y = source.Y * InverseLength;
        result.Z = source.Z * InverseLength;
        result.W = source.W * InverseLength;
    }
}

Quaternion
Quaternion::Normalize(const Quaternion& source) noexcept
{
    Quaternion result;
    Normalize(source, result);
    return result;
}

void Quaternion::Lerp(
    const Quaternion& source1,
    const Quaternion& source2,
    float amount,
    Quaternion& result)
{
    result.X = math::Lerp(source1.X, source2.X, amount);
    result.Y = math::Lerp(source1.Y, source2.Y, amount);
    result.Z = math::Lerp(source1.Z, source2.Z, amount);
    result.W = math::Lerp(source1.W, source2.W, amount);
    result.Normalize();
}

Quaternion
Quaternion::Lerp(
    const Quaternion& source1,
    const Quaternion& source2,
    float amount)
{
    Quaternion result;
    Lerp(source1, source2, amount, result);
    return result;
}

void Quaternion::Slerp(
    const Quaternion& begin,
    const Quaternion& end,
    float amount,
    Quaternion& result)
{
    auto cosAngle = Quaternion::Dot(begin, end);

    const auto angle(std::acos(cosAngle));
    const auto inverseSinAngle = 1 / std::sin(angle);

    const auto coefficient1 = std::sin((1 - amount) * angle) * inverseSinAngle;
    const auto coefficient2 = std::sin(amount * angle) * inverseSinAngle;

    result.X = coefficient1 * begin.X + coefficient2 * end.X;
    result.Y = coefficient1 * begin.Y + coefficient2 * end.Y;
    result.Z = coefficient1 * begin.Z + coefficient2 * end.Z;
    result.W = coefficient1 * begin.W + coefficient2 * end.W;
}

Quaternion
Quaternion::Slerp(
    const Quaternion& begin,
    const Quaternion& end,
    float amount)
{
    Quaternion result;
    Slerp(begin, end, amount, result);
    return result;
}

void Quaternion::Inverse(const Quaternion& source, Quaternion& result)
{
    const auto lengthSquared = source.LengthSquared();
    if (0 < lengthSquared) {
        const auto inverseLengthSquared = 1 / lengthSquared;
        result.X = source.X * -inverseLengthSquared;
        result.Y = source.Y * -inverseLengthSquared;
        result.Z = source.Z * -inverseLengthSquared;
        result.W = source.W * inverseLengthSquared;
    }
    // Failed to calculate inverse
    //result = Identity;
}

Quaternion
Quaternion::Inverse(const Quaternion& source)
{
    Quaternion result;
    Inverse(source, result);
    return result;
}

Vector3
Quaternion::Rotate(const Quaternion& quaternion, const Vector3& vector)
{
    auto xyz = Vector3{quaternion.X, quaternion.Y, quaternion.Z};
    auto t = static_cast<float>(2) * Vector3::Cross(xyz, vector);
    return vector + quaternion.W * t + Vector3::Cross(xyz, t);
}

void Quaternion::CreateFromAxisAngle(const Vector3& axis, const Radian<float>& angle,
    Quaternion& result)
{
    const auto halfAngle(angle.value / 2);
    const auto sinAngle(std::sin(halfAngle));
    const auto cosAngle(std::cos(halfAngle));

    result.X = axis.X * sinAngle;
    result.Y = axis.Y * sinAngle;
    result.Z = axis.Z * sinAngle;
    result.W = cosAngle;
}

Quaternion
Quaternion::CreateFromAxisAngle(const Vector3& axis, const Radian<float>& angle)
{
    Quaternion result;
    CreateFromAxisAngle(axis, angle, result);
    return result;
}

namespace {
template <class MatrixClass>
void CreateFromRotationMatrixImplementation(const MatrixClass& rotation, Quaternion& result)
{
    // NOTE: Algorithm from the article "Quaternion Calculus and Fast Animation"
    // by Ken Shoemake, SIGGRAPH 1987 Course Notes.

    static_assert(std::is_same_v<std::remove_const_t<std::remove_reference_t<decltype(rotation(0, 0))>>, float>);

    const auto trace = rotation(0, 0) + rotation(1, 1) + rotation(2, 2);
    constexpr float half = 0.5f;

    if (trace > 0) {
        auto root = std::sqrt(trace + 1);
        result.W = half * root;
        root = half / root;
        result.X = (rotation(2, 1) - rotation(1, 2)) * root;
        result.Y = (rotation(0, 2) - rotation(2, 0)) * root;
        result.Z = (rotation(1, 0) - rotation(0, 1)) * root;
    }
    else {
        std::size_t i = 0;
        if (rotation(1, 1) > rotation(0, 0))
            i = 1;
        if (rotation(2, 2) > rotation(i, i))
            i = 2;

        // (i, j, k) = (0, 1, 2), (1, 2, 0) or (2, 0, 1).
        //std::size_t const j = (i+1) % 3;
        //std::size_t const k = (i+2) % 3;

        static std::size_t const indices[3] = {1, 2, 0};
        std::size_t const j = indices[i];
        std::size_t const k = indices[j];

        auto root = std::sqrt(rotation(i, i) - rotation(j, j) - rotation(k, k) + 1.0f);
        std::array<float*, 3> const quat = {{&result.X, &result.Y, &result.Z}};
        *quat[i] = half * root;
        root = half / root;
        result.W = (rotation(k, j) - rotation(j, k)) * root;
        *quat[j] = (rotation(j, i) + rotation(i, j)) * root;
        *quat[k] = (rotation(k, i) + rotation(i, k)) * root;
    }
}
} // namespace

void Quaternion::CreateFromRotationMatrix(const Matrix4x4& rotation, Quaternion& result)
{
    CreateFromRotationMatrixImplementation(rotation, result);
}

Quaternion
Quaternion::CreateFromRotationMatrix(const Matrix4x4& rotation)
{
    Quaternion result;
    CreateFromRotationMatrix(rotation, result);
    return result;
}

void Quaternion::CreateFromRotationMatrix(const Matrix3x3& rotation, Quaternion& result)
{
    CreateFromRotationMatrixImplementation(rotation, result);
}

Quaternion
Quaternion::CreateFromRotationMatrix(const Matrix3x3& rotation)
{
    Quaternion result;
    CreateFromRotationMatrix(rotation, result);
    return result;
}

void Quaternion::CreateFromYawPitchRoll(
    const Radian<float>& yaw, const Radian<float>& pitch, const Radian<float>& roll, Quaternion& result)
{
    const auto halfYaw = yaw.value / 2;
    const auto cosYaw = std::cos(halfYaw);
    const auto sinYaw = std::sin(halfYaw);

    const auto halfPitch = pitch.value / 2;
    const auto cosPitch = std::cos(halfPitch);
    const auto sinPitch = std::sin(halfPitch);

    const auto halfRoll = roll.value / 2;
    const auto cosRoll = std::cos(halfRoll);
    const auto sinRoll = std::sin(halfRoll);

    const auto cosPitchSinRoll = cosPitch * sinRoll;
    const auto sinPitchCosRoll = sinPitch * cosRoll;
    const auto cosPitchCosRoll = cosPitch * cosRoll;
    const auto sinPitchSinRoll = sinPitch * sinRoll;

    result.X = (cosYaw * sinPitchCosRoll) + (sinYaw * cosPitchSinRoll);
    result.Y = (sinYaw * cosPitchCosRoll) - (cosYaw * sinPitchSinRoll);
    result.Z = (cosYaw * cosPitchSinRoll) - (sinYaw * sinPitchCosRoll);
    result.W = (cosYaw * cosPitchCosRoll) + (sinYaw * sinPitchSinRoll);
}

Quaternion
Quaternion::CreateFromYawPitchRoll(
    const Radian<float>& yaw, const Radian<float>& pitch, const Radian<float>& roll)
{
    Quaternion result;
    CreateFromYawPitchRoll(yaw, pitch, roll, result);
    return result;
}

Quaternion
Quaternion::Euler(const Vector3& rotation)
{
    return CreateFromYawPitchRoll(rotation.Y, rotation.X, rotation.Z);
}

Vector3
Quaternion::ToEulerAngles(const Quaternion& q) noexcept
{
    const auto ww = q.W * q.W;
    const auto xx = q.X * q.X;
    const auto yy = q.Y * q.Y;
    const auto zz = q.Z * q.Z;

    constexpr auto epsilon = std::numeric_limits<float>::epsilon();
    constexpr auto zero = static_cast<float>(0);
    constexpr auto one = static_cast<float>(1);
    constexpr auto two = static_cast<float>(2);
    constexpr auto half = (static_cast<float>(0.5L) - epsilon);

    const auto lengthSquared = xx + yy + zz + ww;
    const auto singularityTest = q.X * q.Z + q.Y * q.W;
    const auto singularityValue = half * lengthSquared;

    // NOTE: {x, y, z} == {pitch, yaw, roll}
    Vector3 pitchYawRoll;

    if (singularityTest > singularityValue) {
        pitchYawRoll.X = zero;
        pitchYawRoll.Y = math::PiOver2<float>;
        pitchYawRoll.Z = two * std::atan2(q.X, q.W);
    }
    else if (singularityTest < -singularityValue) {
        pitchYawRoll.X = zero;
        pitchYawRoll.Y = -math::PiOver2<float>;
        pitchYawRoll.Z = -two * std::atan2(q.X, q.W);
    }
    else {
        pitchYawRoll.X = std::atan2(two * (-q.Y * q.Z + q.X * q.W), one - two * (xx + yy));
        pitchYawRoll.Y = std::asin(two * (q.X * q.Z + q.Y * q.W));
        pitchYawRoll.Z = std::atan2(two * (-q.X * q.Y + q.Z * q.W), one - two * (yy + zz));
    }
    return pitchYawRoll;
}

const float* Quaternion::Data() const noexcept
{
    return &X;
}

float* Quaternion::Data() noexcept
{
    return &X;
}

[[nodiscard]] Quaternion
operator*(float scaleFactor, const Quaternion& quaternion) noexcept
{
    return Quaternion(
        scaleFactor * quaternion.X,
        scaleFactor * quaternion.Y,
        scaleFactor * quaternion.Z,
        scaleFactor * quaternion.W);
}

} // namespace pomdog
