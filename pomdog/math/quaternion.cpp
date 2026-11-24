// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/quaternion.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/math/matrix3x3.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/simd/f32x4.h"
#include "pomdog/math/simd/vector3_lanes.h"
#include "pomdog/math/trigonometry.h"
#include "pomdog/math/turn.h"
#include "pomdog/math/vector3.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Quaternion::Quaternion() noexcept = default;

Quaternion::Quaternion(f32 xIn, f32 yIn, f32 zIn, f32 wIn) noexcept
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

Quaternion& Quaternion::operator*=(f32 factor) noexcept
{
    x *= factor;
    y *= factor;
    z *= factor;
    w *= factor;
    return *this;
}

Quaternion& Quaternion::operator/=(f32 factor) noexcept
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
    //
    // Each lane adds its terms in the order of the scalar formula
    //
    //   x: w * other.x + x * other.w + y * other.z - z * other.y
    //   y: w * other.y + y * other.w + z * other.x - x * other.z
    //   z: w * other.z + z * other.w + x * other.y - y * other.x
    //   w: w * other.w - x * other.x - y * other.y - z * other.z
    //
    // ARM64 uses fused multiply-add for the accumulation. The signs
    // (1, 1, 1, -1) turn the second and third additions into subtractions in lane w.
    namespace simd = math::simd;
    using simd::F32x4;

    // NOTE: Lanes lets std::bit_cast move the four members of Quaternion into
    // and out of F32x4 without reading through a pointer to x. Compilers turn
    // each copy into one 16-byte load or store.
    struct Lanes final {
        f32 values[4];
    };

    static constexpr f32 signValues[4] = {1.0f, 1.0f, 1.0f, -1.0f};
    const F32x4 signs = simd::load<F32x4>(signValues);
    const F32x4 a = simd::load<F32x4>(std::bit_cast<Lanes>(*this).values);
    const F32x4 b = simd::load<F32x4>(std::bit_cast<Lanes>(other).values);

    F32x4 r = simd::multiply(simd::swizzle<3, 3, 3, 3>(a), b);
    r = simd::addProduct(r, simd::multiply(simd::swizzle<0, 1, 2, 0>(a), signs), simd::swizzle<3, 3, 3, 0>(b));
    r = simd::addProduct(r, simd::multiply(simd::swizzle<1, 2, 0, 1>(a), signs), simd::swizzle<2, 0, 1, 1>(b));
    r = simd::subtractProduct(r, simd::swizzle<2, 0, 1, 2>(a), simd::swizzle<1, 2, 0, 2>(b));

    Lanes result;
    simd::store(result.values, r);
    return std::bit_cast<Quaternion>(result);
}

Quaternion Quaternion::operator*(f32 factor) const noexcept
{
    return Quaternion{
        x * factor,
        y * factor,
        z * factor,
        w * factor,
    };
}

Quaternion Quaternion::operator/(f32 factor) const noexcept
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

const f32* Quaternion::data() const noexcept
{
    return &x;
}

f32* Quaternion::data() noexcept
{
    return &x;
}

Quaternion
Quaternion::createFromAxisAngle(const Vector3& axis, const Radian32& angle)
{
    const auto halfAngle(angle.get() / 2);
    const auto sinAngle(std::sin(halfAngle));
    const auto cosAngle(std::cos(halfAngle));

    return Quaternion{
        axis.x * sinAngle,
        axis.y * sinAngle,
        axis.z * sinAngle,
        cosAngle,
    };
}

Quaternion
Quaternion::createFromAxisAngle(const Vector3& axis, const Turn32& angle) noexcept
{
    // NOTE: A quaternion holds half the angle, and sincospi takes half turns,
    // so sincospi(t) gives the sine and cosine of half of t turns.
    const auto [sinAngle, cosAngle] = math::sincospi(angle.get());

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
createFromRotationMatrixImpl(const MatrixClass& rotation)
{
    // NOTE: Algorithm from the article "Quaternion Calculus and Fast Animation"
    // by Ken Shoemake, SIGGRAPH 1987 Course Notes.

    static_assert(std::is_same_v<std::remove_const_t<std::remove_reference_t<decltype(rotation(0, 0))>>, f32>);

    const auto trace = rotation(0, 0) + rotation(1, 1) + rotation(2, 2);
    constexpr f32 half = 0.5f;

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

    i32 i = 0;
    if (rotation(1, 1) > rotation(0, 0)) {
        i = 1;
    }
    if (rotation(2, 2) > rotation(i, i)) {
        i = 2;
    }

    // (i, j, k) = (0, 1, 2), (1, 2, 0) or (2, 0, 1).
    // i32 const j = (i+1) % 3;
    // i32 const k = (i+2) % 3;

    static i32 const indices[3] = {1, 2, 0};
    i32 const j = indices[i];
    i32 const k = indices[j];

    const auto root = std::sqrt(rotation(i, i) - rotation(j, j) - rotation(k, k) + 1.0f);
    const auto factor = half / root;

    Quaternion result;
    const std::array<f32*, 3> quat = {{&result.x, &result.y, &result.z}};
    *quat[i] = half * root;
    *quat[j] = (rotation(j, i) + rotation(i, j)) * factor;
    *quat[k] = (rotation(k, i) + rotation(i, k)) * factor;
    result.w = (rotation(k, j) - rotation(j, k)) * factor;
    return result;
}
} // namespace

Quaternion
Quaternion::createFromRotationMatrix(const Matrix4x4& rotation)
{
    return createFromRotationMatrixImpl(rotation);
}

Quaternion
Quaternion::createFromRotationMatrix(const Matrix3x3& rotation)
{
    return createFromRotationMatrixImpl(rotation);
}

namespace {

/// Returns the quaternion for a yaw, pitch, and roll, given the sines and
/// cosines of half of each angle.
[[nodiscard]] Quaternion
createFromHalfYawPitchRoll(
    f32 sinYaw, f32 cosYaw,
    f32 sinPitch, f32 cosPitch,
    f32 sinRoll, f32 cosRoll) noexcept
{
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

} // namespace

Quaternion
Quaternion::createFromYawPitchRoll(const Radian32& yaw, const Radian32& pitch, const Radian32& roll)
{
    const auto halfYaw = yaw.get() / 2;
    const auto cosYaw = std::cos(halfYaw);
    const auto sinYaw = std::sin(halfYaw);

    const auto halfPitch = pitch.get() / 2;
    const auto cosPitch = std::cos(halfPitch);
    const auto sinPitch = std::sin(halfPitch);

    const auto halfRoll = roll.get() / 2;
    const auto cosRoll = std::cos(halfRoll);
    const auto sinRoll = std::sin(halfRoll);

    return createFromHalfYawPitchRoll(sinYaw, cosYaw, sinPitch, cosPitch, sinRoll, cosRoll);
}

Quaternion
Quaternion::createFromYawPitchRoll(const Turn32& yaw, const Turn32& pitch, const Turn32& roll) noexcept
{
    // NOTE: A quaternion holds half the angle, and sincospi takes half turns,
    // so sincospi(t) gives the sine and cosine of half of t turns.
    const auto [sinYaw, cosYaw] = math::sincospi(yaw.get());
    const auto [sinPitch, cosPitch] = math::sincospi(pitch.get());
    const auto [sinRoll, cosRoll] = math::sincospi(roll.get());
    return createFromHalfYawPitchRoll(sinYaw, cosYaw, sinPitch, cosPitch, sinRoll, cosRoll);
}

Quaternion
Quaternion::euler(const Vector3& rotation)
{
    return createFromYawPitchRoll(Radian32{rotation.y}, Radian32{rotation.x}, Radian32{rotation.z});
}

Vector3
Quaternion::toEulerAngles(const Quaternion& q) noexcept
{
    const auto ww = q.w * q.w;
    const auto xx = q.x * q.x;
    const auto yy = q.y * q.y;
    const auto zz = q.z * q.z;

    constexpr auto epsilon = std::numeric_limits<f32>::epsilon();
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
        pitchYawRoll.y = math::PiOver2<f32>;
        pitchYawRoll.z = two * std::atan2(q.x, q.w);
    }
    else if (singularityTest < -singularityValue) {
        pitchYawRoll.x = zero;
        pitchYawRoll.y = -math::PiOver2<f32>;
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
Quaternion::createIdentity() noexcept
{
    return Quaternion{0.0f, 0.0f, 0.0f, 1.0f};
}

[[nodiscard]] Quaternion
operator*(f32 factor, const Quaternion& quaternion) noexcept
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

[[nodiscard]] f32
length(const Quaternion& q) noexcept
{
    return std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

[[nodiscard]] f32
lengthSquared(const Quaternion& q) noexcept
{
    return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
}

[[nodiscard]] f32
dot(const Quaternion& a, const Quaternion& b) noexcept
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

[[nodiscard]] Quaternion
normalize(const Quaternion& source) noexcept
{
    const auto length = math::length(source);

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
lerp(const Quaternion& source1, const Quaternion& source2, f32 amount)
{
    return math::normalize(Quaternion{
        math::lerp(source1.x, source2.x, amount),
        math::lerp(source1.y, source2.y, amount),
        math::lerp(source1.z, source2.z, amount),
        math::lerp(source1.w, source2.w, amount),
    });
}

[[nodiscard]] Quaternion
slerp(const Quaternion& begin, const Quaternion& end, f32 amount)
{
    // NOTE: `begin` and `end` must be normalized quaternions.
    POMDOG_ASSERT(!std::isnan(math::lengthSquared(begin)));
    POMDOG_ASSERT(!std::isnan(math::lengthSquared(end)));

    const auto cosAngle = math::dot(begin, end);
    POMDOG_ASSERT(!std::isnan(cosAngle));
    POMDOG_ASSERT(!std::isinf(cosAngle));

    const auto angle = std::acos(cosAngle);
    POMDOG_ASSERT(!std::isnan(angle));
    POMDOG_ASSERT(!std::isinf(angle));

    const auto sinAngle = std::sin(angle);
    POMDOG_ASSERT(std::isnormal(sinAngle));

    const auto inverseSinAngle = 1.0f / sinAngle;
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
invert(const Quaternion& source)
{
    const auto lengthSquared = math::lengthSquared(source);
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
rotate(const Quaternion& quaternion, const Vector3& vector)
{
    namespace simd = math::simd;
    using simd::F32x4;

    // NOTE: Computes vector + w * t + cross(xyz, t) with t = 2 * cross(xyz, vector)
    // in F32x4, with the operations of the scalar formula in the same order.
    // After inlining into loops, LLVM's default vectorizers turned the scalar
    // formula into slower code on wasm, and Clang's SLP vectorizer did on x86-64.
    struct Lanes final {
        f32 values[4];
    };
    const F32x4 q = simd::load<F32x4>(std::bit_cast<Lanes>(quaternion).values);
    const F32x4 v = simd::loadVector3(vector);
    const F32x4 t = simd::multiply(simd::splat<F32x4>(2.0f), simd::cross3(q, v));
    return simd::storeVector3(simd::add(simd::add(v, simd::multiply(simd::swizzle<3, 3, 3, 3>(q), t)), simd::cross3(q, t)));
}

[[nodiscard]] Quaternion
abs(const Quaternion& source) noexcept
{
    return Quaternion{
        std::abs(source.x),
        std::abs(source.y),
        std::abs(source.z),
        std::abs(source.w),
    };
}

[[nodiscard]] Quaternion
clamp(const Quaternion& source, const Quaternion& min, const Quaternion& max) noexcept
{
    return Quaternion{
        std::clamp(source.x, min.x, max.x),
        std::clamp(source.y, min.y, max.y),
        std::clamp(source.z, min.z, max.z),
        std::clamp(source.w, min.w, max.w),
    };
}

[[nodiscard]] Quaternion
saturate(const Quaternion& source) noexcept
{
    return math::clamp(source,
        Quaternion{0.0f, 0.0f, 0.0f, 0.0f},
        Quaternion{1.0f, 1.0f, 1.0f, 1.0f});
}

[[nodiscard]] Quaternion
smoothstep(const Quaternion& source1, const Quaternion& source2, f32 amount)
{
    return Quaternion{
        math::smoothstep(source1.x, source2.x, amount),
        math::smoothstep(source1.y, source2.y, amount),
        math::smoothstep(source1.z, source2.z, amount),
        math::smoothstep(source1.w, source2.w, amount),
    };
}

} // namespace pomdog::math
