// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/quaternion.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/math.h"
#include "pomdog/math/matrix3x3.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector3.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cmath>
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Quaternion::Quaternion() noexcept = default;

Quaternion::Quaternion(float xIn, float yIn, float zIn, float wIn) noexcept
    : x(xIn)
    , y(yIn)
    , z(zIn)
    , w(wIn)
{
}

Quaternion& Quaternion::operator+=(const Quaternion& other) noexcept
{
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;
    return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion& other) noexcept
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;
    return *this;
}

Quaternion& Quaternion::operator*=(const Quaternion& other) noexcept
{
    return (*this) = (*this) * other;
}

Quaternion& Quaternion::operator*=(float factor) noexcept
{
    x *= factor;
    y *= factor;
    z *= factor;
    w *= factor;
    return *this;
}

Quaternion& Quaternion::operator/=(float factor) noexcept
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

Quaternion Quaternion::operator+() const noexcept
{
    return *this;
}

Quaternion Quaternion::operator-() const noexcept
{
    return Quaternion(-x, -y, -z, -w);
}

Quaternion Quaternion::operator+(const Quaternion& other) const noexcept
{
    return Quaternion{
        x + other.x,
        y + other.y,
        z + other.z,
        w + other.w,
    };
}

Quaternion Quaternion::operator-(const Quaternion& other) const noexcept
{
    return Quaternion{
        x - other.x,
        y - other.y,
        z - other.z,
        w - other.w,
    };
}

Quaternion Quaternion::operator*(const Quaternion& other) const noexcept
{
    // NOTE: Quaternion{
    //           w * other.xyz + xyz * other.w + Vector3::Cross(xyz, other.xyz),
    //           w * other.w - Vector3::Dot(xyz, other.xyz)};
    return Quaternion{
        w * other.x + x * other.w + y * other.z - z * other.y,
        w * other.y + y * other.w + z * other.x - x * other.z,
        w * other.z + z * other.w + x * other.y - y * other.x,
        w * other.w - x * other.x - y * other.y - z * other.z,
    };
}

Quaternion Quaternion::operator*(float factor) const noexcept
{
    return Quaternion{
        x * factor,
        y * factor,
        z * factor,
        w * factor,
    };
}

Quaternion Quaternion::operator/(float factor) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    return Quaternion{
        x / factor,
        y / factor,
        z / factor,
        w / factor,
    };
}

bool Quaternion::operator==(const Quaternion& other) const noexcept
{
    return (x == other.x) && (y == other.y) && (z == other.z) && (w == other.w);
}

bool Quaternion::operator!=(const Quaternion& other) const noexcept
{
    return (x != other.x) || (y != other.y) || (z != other.z) || (w != other.w);
}

const float* Quaternion::Data() const noexcept
{
    return &x;
}

float* Quaternion::Data() noexcept
{
    return &x;
}

Quaternion
Quaternion::CreateFromAxisAngle(const Vector3& axis, const Radian<float>& angle)
{
    const auto halfAngle(angle.value / 2);
    const auto sinAngle(std::sin(halfAngle));
    const auto cosAngle(std::cos(halfAngle));

    return Quaternion{
        axis.x * sinAngle,
        axis.y * sinAngle,
        axis.z * sinAngle,
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
    std::array<float*, 3> const quat = {{&result.x, &result.y, &result.z}};
    *quat[i] = half * root;
    *quat[j] = (rotation(j, i) + rotation(i, j)) * factor;
    *quat[k] = (rotation(k, i) + rotation(i, k)) * factor;
    result.w = (rotation(k, j) - rotation(j, k)) * factor;
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
    return CreateFromYawPitchRoll(rotation.y, rotation.x, rotation.z);
}

Vector3
Quaternion::ToEulerAngles(const Quaternion& q) noexcept
{
    const auto ww = q.w * q.w;
    const auto xx = q.x * q.x;
    const auto yy = q.y * q.y;
    const auto zz = q.z * q.z;

    constexpr auto epsilon = std::numeric_limits<float>::epsilon();
    constexpr auto zero = 0.0f;
    constexpr auto one = 1.0f;
    constexpr auto two = 2.0f;
    constexpr auto half = 0.5f - epsilon;

    const auto lengthSquared = xx + yy + zz + ww;
    const auto singularityTest = q.x * q.z + q.y * q.w;
    const auto singularityValue = half * lengthSquared;

    // NOTE: {x, y, z} == {pitch, yaw, roll}
    Vector3 pitchYawRoll;

    if (singularityTest > singularityValue) {
        pitchYawRoll.x = zero;
        pitchYawRoll.y = math::PiOver2<float>;
        pitchYawRoll.z = two * std::atan2(q.x, q.w);
    }
    else if (singularityTest < -singularityValue) {
        pitchYawRoll.x = zero;
        pitchYawRoll.y = -math::PiOver2<float>;
        pitchYawRoll.z = -two * std::atan2(q.x, q.w);
    }
    else {
        pitchYawRoll.x = std::atan2(two * (-q.y * q.z + q.x * q.w), one - two * (xx + yy));
        pitchYawRoll.y = std::asin(two * (q.x * q.z + q.y * q.w));
        pitchYawRoll.z = std::atan2(two * (-q.x * q.y + q.z * q.w), one - two * (yy + zz));
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
        factor * quaternion.x,
        factor * quaternion.y,
        factor * quaternion.z,
        factor * quaternion.w,
    };
}

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] float Length(const Quaternion& q) noexcept
{
    return std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

[[nodiscard]] float LengthSquared(const Quaternion& q) noexcept
{
    return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
}

[[nodiscard]] float Dot(const Quaternion& a, const Quaternion& b) noexcept
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
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
        math::Lerp(source1.x, source2.x, amount),
        math::Lerp(source1.y, source2.y, amount),
        math::Lerp(source1.z, source2.z, amount),
        math::Lerp(source1.w, source2.w, amount),
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
        coefficient1 * begin.x + coefficient2 * end.x,
        coefficient1 * begin.y + coefficient2 * end.y,
        coefficient1 * begin.z + coefficient2 * end.z,
        coefficient1 * begin.w + coefficient2 * end.w,
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
            source.x * -inverseLenSq,
            source.y * -inverseLenSq,
            source.z * -inverseLenSq,
            source.w * inverseLenSq,
        };
    }

    // FIXME: Add error handling
    return source;
}

[[nodiscard]] Vector3
Rotate(const Quaternion& quaternion, const Vector3& vector)
{
    const auto xyz = Vector3{quaternion.x, quaternion.y, quaternion.z};
    const auto t = 2.0f * math::Cross(xyz, vector);
    return vector + quaternion.w * t + math::Cross(xyz, t);
}

} // namespace pomdog::math
