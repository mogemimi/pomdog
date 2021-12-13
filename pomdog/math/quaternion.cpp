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

Quaternion& Quaternion::operator*=(float factor) noexcept
{
    X *= factor;
    Y *= factor;
    Z *= factor;
    W *= factor;
    return *this;
}

Quaternion& Quaternion::operator/=(float factor) noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    X /= factor;
    W /= factor;
    Z /= factor;
    W /= factor;
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
    return Quaternion{
        W * other.X + X * other.W + Y * other.Z - Z * other.Y,
        W * other.Y + Y * other.W + Z * other.X - X * other.Z,
        W * other.Z + Z * other.W + X * other.Y - Y * other.X,
        W * other.W - X * other.X - Y * other.Y - Z * other.Z,
    };
}

Quaternion Quaternion::operator*(float factor) const noexcept
{
    return Quaternion{
        X * factor,
        Y * factor,
        Z * factor,
        W * factor,
    };
}

Quaternion Quaternion::operator/(float factor) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    return Quaternion{
        X / factor,
        Y / factor,
        Z / factor,
        W / factor,
    };
}

bool Quaternion::operator==(const Quaternion& other) const noexcept
{
    return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
}

bool Quaternion::operator!=(const Quaternion& other) const noexcept
{
    return X != other.X || Y != other.Y || Z != other.Z || W != other.W;
}

const float* Quaternion::Data() const noexcept
{
    return &X;
}

float* Quaternion::Data() noexcept
{
    return &X;
}

Quaternion
Quaternion::CreateFromAxisAngle(const Vector3& axis, const Radian<float>& angle)
{
    const auto halfAngle(angle.value / 2);
    const auto sinAngle(std::sin(halfAngle));
    const auto cosAngle(std::cos(halfAngle));

    return Quaternion{
        axis.X * sinAngle,
        axis.Y * sinAngle,
        axis.Z * sinAngle,
        cosAngle,
    };
}

namespace {
template <class MatrixClass>
[[nodiscard]] Quaternion
CreateFromRotationMatrixImpl(const MatrixClass& rotation)
{
    // NOTE: Algorithm from the article "Quaternion Calculus and Fast Animation"
    // by Ken Shoemake, SIGGRAPH 1987 Course Notes.

    static_assert(std::is_same_v<std::remove_const_t<std::remove_reference_t<decltype(rotation(0, 0))>>, float>);

    const auto trace = rotation(0, 0) + rotation(1, 1) + rotation(2, 2);
    constexpr float half = 0.5f;

    if (trace > 0) {
        const auto root = std::sqrt(trace + 1.0f);
        const auto factor = half / root;
        return Quaternion{
            (rotation(2, 1) - rotation(1, 2)) * factor,
            (rotation(0, 2) - rotation(2, 0)) * factor,
            (rotation(1, 0) - rotation(0, 1)) * factor,
            half * root,
        };
    }

    std::size_t i = 0;
    if (rotation(1, 1) > rotation(0, 0)) {
        i = 1;
    }
    if (rotation(2, 2) > rotation(i, i)) {
        i = 2;
    }

    // (i, j, k) = (0, 1, 2), (1, 2, 0) or (2, 0, 1).
    //std::size_t const j = (i+1) % 3;
    //std::size_t const k = (i+2) % 3;

    static std::size_t const indices[3] = {1, 2, 0};
    std::size_t const j = indices[i];
    std::size_t const k = indices[j];

    const auto root = std::sqrt(rotation(i, i) - rotation(j, j) - rotation(k, k) + 1.0f);
    const auto factor = half / root;

    Quaternion result;
    std::array<float*, 3> const quat = {{&result.X, &result.Y, &result.Z}};
    *quat[i] = half * root;
    *quat[j] = (rotation(j, i) + rotation(i, j)) * factor;
    *quat[k] = (rotation(k, i) + rotation(i, k)) * factor;
    result.W = (rotation(k, j) - rotation(j, k)) * factor;
    return result;
}
} // namespace

Quaternion
Quaternion::CreateFromRotationMatrix(const Matrix4x4& rotation)
{
    return CreateFromRotationMatrixImpl(rotation);
}

Quaternion
Quaternion::CreateFromRotationMatrix(const Matrix3x3& rotation)
{
    return CreateFromRotationMatrixImpl(rotation);
}

Quaternion
Quaternion::CreateFromYawPitchRoll(const Radian<float>& yaw, const Radian<float>& pitch, const Radian<float>& roll)
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

    return Quaternion{
        (cosYaw * sinPitchCosRoll) + (sinYaw * cosPitchSinRoll),
        (sinYaw * cosPitchCosRoll) - (cosYaw * sinPitchSinRoll),
        (cosYaw * cosPitchSinRoll) - (sinYaw * sinPitchCosRoll),
        (cosYaw * cosPitchCosRoll) + (sinYaw * sinPitchSinRoll),
    };
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
    constexpr auto zero = 0.0f;
    constexpr auto one = 1.0f;
    constexpr auto two = 2.0f;
    constexpr auto half = 0.5f - epsilon;

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

Quaternion
Quaternion::Identity() noexcept
{
    return Quaternion{0.0f, 0.0f, 0.0f, 1.0f};
}

[[nodiscard]] Quaternion
operator*(float factor, const Quaternion& quaternion) noexcept
{
    return Quaternion{
        factor * quaternion.X,
        factor * quaternion.Y,
        factor * quaternion.Z,
        factor * quaternion.W,
    };
}

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] float Length(const Quaternion& q) noexcept
{
    return std::sqrt(q.X * q.X + q.Y * q.Y + q.Z * q.Z + q.W * q.W);
}

[[nodiscard]] float LengthSquared(const Quaternion& q) noexcept
{
    return q.X * q.X + q.Y * q.Y + q.Z * q.Z + q.W * q.W;
}

[[nodiscard]] float Dot(const Quaternion& a, const Quaternion& b) noexcept
{
    return a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W;
}

[[nodiscard]] Quaternion
Normalize(const Quaternion& source) noexcept
{
    const auto length = math::Length(source);

    if (length > std::numeric_limits<decltype(length)>::epsilon()) {
        POMDOG_ASSERT(std::fpclassify(length) != FP_ZERO);
        POMDOG_ASSERT(std::fpclassify(length) != FP_NAN);
        POMDOG_ASSERT(std::fpclassify(length) != FP_INFINITE);
        const auto inverseLength = 1.0f / length;
        return source * inverseLength;
    }

    // FIXME: Add error handling
    return source;
}

[[nodiscard]] Quaternion
Lerp(const Quaternion& source1, const Quaternion& source2, float amount)
{
    return math::Normalize(Quaternion{
        math::Lerp(source1.X, source2.X, amount),
        math::Lerp(source1.Y, source2.Y, amount),
        math::Lerp(source1.Z, source2.Z, amount),
        math::Lerp(source1.W, source2.W, amount),
    });
}

[[nodiscard]] Quaternion
Slerp(const Quaternion& begin, const Quaternion& end, float amount)
{
    const auto cosAngle = math::Dot(begin, end);

    const auto angle(std::acos(cosAngle));
    const auto inverseSinAngle = 1.0f / std::sin(angle);

    const auto coefficient1 = std::sin((1.0f - amount) * angle) * inverseSinAngle;
    const auto coefficient2 = std::sin(amount * angle) * inverseSinAngle;

    return Quaternion{
        coefficient1 * begin.X + coefficient2 * end.X,
        coefficient1 * begin.Y + coefficient2 * end.Y,
        coefficient1 * begin.Z + coefficient2 * end.Z,
        coefficient1 * begin.W + coefficient2 * end.W,
    };
}

[[nodiscard]] Quaternion
Invert(const Quaternion& source)
{
    const auto lengthSquared = math::LengthSquared(source);
    if (lengthSquared > std::numeric_limits<decltype(lengthSquared)>::epsilon()) {
        POMDOG_ASSERT(std::fpclassify(lengthSquared) != FP_ZERO);
        POMDOG_ASSERT(std::fpclassify(lengthSquared) != FP_NAN);
        POMDOG_ASSERT(std::fpclassify(lengthSquared) != FP_INFINITE);
        const auto inverseLenSq = 1.0f / lengthSquared;
        return Quaternion{
            source.X * -inverseLenSq,
            source.Y * -inverseLenSq,
            source.Z * -inverseLenSq,
            source.W * inverseLenSq,
        };
    }

    // FIXME: Add error handling
    return source;
}

[[nodiscard]] Vector3
Rotate(const Quaternion& quaternion, const Vector3& vector)
{
    const auto xyz = Vector3{quaternion.X, quaternion.Y, quaternion.Z};
    const auto t = 2.0f * Vector3::Cross(xyz, vector);
    return vector + quaternion.W * t + Vector3::Cross(xyz, t);
}

} // namespace pomdog::math
