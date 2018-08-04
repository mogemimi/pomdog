// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/detail/FloatingPointQuaternion.hpp"
#include "Pomdog/Math/Radian.hpp"
#include "Pomdog/Math/detail/FloatingPointMatrix3x3.hpp"
#include "Pomdog/Math/detail/FloatingPointMatrix4x4.hpp"
#include "Pomdog/Math/detail/FloatingPointVector3.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <array>
#include <cfloat>
#include <cmath>
#include <limits>
#include <utility>

namespace Pomdog {
namespace Detail {

template <typename T>
FloatingPointQuaternion<T>::FloatingPointQuaternion(T x, T y, T z, T w) noexcept
    : X(x)
    , Y(y)
    , Z(z)
    , W(w)
{
}

template <typename T>
FloatingPointQuaternion<T> & FloatingPointQuaternion<T>::operator+=(const FloatingPointQuaternion& other) noexcept
{
    X += other.X;
    Y += other.Y;
    Z += other.Z;
    W += other.W;
    return *this;
}

template <typename T>
FloatingPointQuaternion<T> & FloatingPointQuaternion<T>::operator-=(const FloatingPointQuaternion& other) noexcept
{
    X -= other.X;
    Y -= other.Y;
    Z -= other.Z;
    W -= other.W;
    return *this;
}

template <typename T>
FloatingPointQuaternion<T> & FloatingPointQuaternion<T>::operator*=(const FloatingPointQuaternion& other) noexcept
{
    return (*this) = (*this) * other;
}

template <typename T>
FloatingPointQuaternion<T> & FloatingPointQuaternion<T>::operator*=(T scaleFactor) noexcept
{
    X *= scaleFactor;
    Y *= scaleFactor;
    Z *= scaleFactor;
    W *= scaleFactor;
    return *this;
}

template <typename T>
FloatingPointQuaternion<T> & FloatingPointQuaternion<T>::operator/=(T scaleFactor)
{
    POMDOG_ASSERT(scaleFactor != 0);
    X /= scaleFactor;
    W /= scaleFactor;
    Z /= scaleFactor;
    W /= scaleFactor;
    return *this;
}

template <typename T>
FloatingPointQuaternion<T> FloatingPointQuaternion<T>::operator+() const noexcept
{
    return *this;
}

template <typename T>
FloatingPointQuaternion<T> FloatingPointQuaternion<T>::operator-() const noexcept
{
    return FloatingPointQuaternion(-X, -Y, -Z, -W);
}

template <typename T>
FloatingPointQuaternion<T> FloatingPointQuaternion<T>::operator+(const FloatingPointQuaternion& other) const noexcept
{
    return FloatingPointQuaternion(
        X + other.X,
        Y + other.Y,
        Z + other.Z,
        W + other.W);
}

template <typename T>
FloatingPointQuaternion<T> FloatingPointQuaternion<T>::operator-(const FloatingPointQuaternion& other) const noexcept
{
    return FloatingPointQuaternion(
        X - other.X,
        Y - other.Y,
        Z - other.Z,
        W - other.W);
}

template <typename T>
FloatingPointQuaternion<T> FloatingPointQuaternion<T>::operator*(const FloatingPointQuaternion& other) const noexcept
{
    // Quaternion(
    //     w * other.Xyz + xyz * other.W + Vector3::Cross(this->xyz, other.Xyz),
    //     w * other.W - Vector3::Dot(this->xyz, other.Xyz));
    return FloatingPointQuaternion(
        W * other.X + X * other.W + Y * other.Z - Z * other.Y,
        W * other.Y + Y * other.W + Z * other.X - X * other.Z,
        W * other.Z + Z * other.W + X * other.Y - Y * other.X,
        W * other.W - X * other.X - Y * other.Y - Z * other.Z);
}

template <typename T>
FloatingPointQuaternion<T> FloatingPointQuaternion<T>::operator*(T scaleFactor) const noexcept
{
    return FloatingPointQuaternion(
        X * scaleFactor,
        Y * scaleFactor,
        Z * scaleFactor,
        W * scaleFactor);
}

template <typename T>
FloatingPointQuaternion<T> FloatingPointQuaternion<T>::operator/(T scaleFactor) const
{
    POMDOG_ASSERT(scaleFactor != 0);
    return FloatingPointQuaternion(
        X / scaleFactor,
        Y / scaleFactor,
        Z / scaleFactor,
        W / scaleFactor);
}

template <typename T>
bool FloatingPointQuaternion<T>::operator==(const FloatingPointQuaternion& other) const noexcept
{
    return X == other.X
        && Y == other.Y
        && Z == other.Z
        && W == other.W;
}

template <typename T>
bool FloatingPointQuaternion<T>::operator!=(const FloatingPointQuaternion& other) const noexcept
{
    return X != other.X
        || Y != other.Y
        || Z != other.Z
        || W != other.W;
}

template <typename T>
T FloatingPointQuaternion<T>::Length() const noexcept
{
    return std::sqrt(X * X + Y * Y + Z * Z + W * W);
}

template <typename T>
T FloatingPointQuaternion<T>::LengthSquared() const noexcept
{
    return X * X + Y * Y + Z * Z + W * W;
}

template <typename T>
T FloatingPointQuaternion<T>::Dot(const FloatingPointQuaternion& a, const FloatingPointQuaternion& b) noexcept
{
    return a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W;
}

template <typename T>
void
FloatingPointQuaternion<T>::Normalize() noexcept
{
    *this = Normalize(*this);
}

template <typename T>
void
FloatingPointQuaternion<T>::Normalize(const FloatingPointQuaternion& source, FloatingPointQuaternion & result) noexcept
{
    auto const length = source.Length();

    if (length > std::numeric_limits<decltype(length)>::epsilon()) {
        auto const InverseLength = 1 / length;
        result.X = source.X * InverseLength;
        result.Y = source.Y * InverseLength;
        result.Z = source.Z * InverseLength;
        result.W = source.W * InverseLength;
    }
}

template <typename T>
FloatingPointQuaternion<T>
FloatingPointQuaternion<T>::Normalize(const FloatingPointQuaternion& source) noexcept
{
    FloatingPointQuaternion result;
    Normalize(source, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointQuaternion<T>::Slerp(const FloatingPointQuaternion& begin, const FloatingPointQuaternion& end, T amount,
    FloatingPointQuaternion & result)
{
    auto cosAngle = FloatingPointQuaternion::Dot(begin, end);

    auto const angle(std::acos(cosAngle));
    auto const inverseSinAngle = 1 / std::sin(angle);

    auto const coefficient1 = std::sin((1 - amount) * angle) * inverseSinAngle;
    auto const coefficient2 = std::sin(amount * angle) * inverseSinAngle;

    result.X = coefficient1 * begin.X + coefficient2 * end.X;
    result.Y = coefficient1 * begin.Y + coefficient2 * end.Y;
    result.Z = coefficient1 * begin.Z + coefficient2 * end.Z;
    result.W = coefficient1 * begin.W + coefficient2 * end.W;
}

template <typename T>
FloatingPointQuaternion<T>
FloatingPointQuaternion<T>::Slerp(const FloatingPointQuaternion& begin, const FloatingPointQuaternion& end, T amount)
{
    FloatingPointQuaternion result;
    Slerp(begin, end, amount, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointQuaternion<T>::Inverse(const FloatingPointQuaternion& source, FloatingPointQuaternion & result)
{
    auto const lengthSquared = source.LengthSquared();
    if (0 < lengthSquared) {
        auto const inverseLengthSquared = 1 / lengthSquared;
        result.X = source.X * -inverseLengthSquared;
        result.Y = source.Y * -inverseLengthSquared;
        result.Z = source.Z * -inverseLengthSquared;
        result.W = source.W *  inverseLengthSquared;
    }
    // Failed to calculate inverse
    //result = Identity;
}

template <typename T>
FloatingPointQuaternion<T>
FloatingPointQuaternion<T>::Inverse(const FloatingPointQuaternion& source)
{
    FloatingPointQuaternion result;
    Inverse(source, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointQuaternion<T>::CreateFromAxisAngle(const FloatingPointVector3<T>& axis, const Radian<T>& angle,
    FloatingPointQuaternion & result)
{
    auto const halfAngle(angle.value / 2);
    auto const sinAngle(std::sin(halfAngle));
    auto const cosAngle(std::cos(halfAngle));

    result.X = axis.X * sinAngle;
    result.Y = axis.Y * sinAngle;
    result.Z = axis.Z * sinAngle;
    result.W = cosAngle;
}

template <typename T>
FloatingPointQuaternion<T>
FloatingPointQuaternion<T>::CreateFromAxisAngle(const FloatingPointVector3<T>& axis, const Radian<T>& angle)
{
    FloatingPointQuaternion result;
    CreateFromAxisAngle(axis, angle, result);
    return std::move(result);
}

template <class MatrixClass, typename T> static
void CreateFromRotationMatrixImplementation(const MatrixClass& rotation, FloatingPointQuaternion<T> & result)
{
    // Algorithm from the article "Quaternion Calculus and Fast Animation"
    // by Ken Shoemake, SIGGRAPH 1987 Course Notes.

    static_assert(std::is_same<decltype(rotation(0, 0)), const T&>::value ||
        std::is_same<decltype(rotation(0, 0)), T>::value, "");

    auto const trace = rotation(0, 0) + rotation(1, 1) + rotation(2, 2);
    constexpr T half = T{0.5};

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

        static std::size_t const indices[3] = { 1, 2, 0 };
        std::size_t const j = indices[i];
        std::size_t const k = indices[j];

        auto root = std::sqrt(rotation(i, i) - rotation(j, j) - rotation(k, k) + T{1});
        std::array<T*, 3> const quat = {{ &result.X, &result.Y, &result.Z }};
        *quat[i] = half * root;
        root = half / root;
        result.W = (rotation(k, j) - rotation(j, k)) * root;
        *quat[j] = (rotation(j, i) + rotation(i, j)) * root;
        *quat[k] = (rotation(k, i) + rotation(i, k)) * root;
    }
}

template <typename T>
void
FloatingPointQuaternion<T>::CreateFromRotationMatrix(const FloatingPointMatrix4x4<T>& rotation, FloatingPointQuaternion & result)
{
    CreateFromRotationMatrixImplementation(rotation, result);
}

template <typename T>
FloatingPointQuaternion<T>
FloatingPointQuaternion<T>::CreateFromRotationMatrix(const FloatingPointMatrix4x4<T>& rotation)
{
    FloatingPointQuaternion result;
    CreateFromRotationMatrix(rotation, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointQuaternion<T>::CreateFromRotationMatrix(const FloatingPointMatrix3x3<T>& rotation, FloatingPointQuaternion & result)
{
    CreateFromRotationMatrixImplementation(rotation, result);
}

template <typename T>
FloatingPointQuaternion<T>
FloatingPointQuaternion<T>::CreateFromRotationMatrix(const FloatingPointMatrix3x3<T>& rotation)
{
    FloatingPointQuaternion result;
    CreateFromRotationMatrix(rotation, result);
    return result;
}

template <typename T>
void
FloatingPointQuaternion<T>::CreateFromYawPitchRoll(
    const Radian<T>& yaw, const Radian<T>& pitch, const Radian<T>& roll, FloatingPointQuaternion & result)
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

template <typename T>
FloatingPointQuaternion<T>
FloatingPointQuaternion<T>::CreateFromYawPitchRoll(
    const Radian<T>& yaw, const Radian<T>& pitch, const Radian<T>& roll)
{
    FloatingPointQuaternion result;
    CreateFromYawPitchRoll(yaw, pitch, roll, result);
    return std::move(result);
}

template <typename T>
const T* FloatingPointQuaternion<T>::Data() const noexcept
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number");
    return &X;
}

template <typename T>
T* FloatingPointQuaternion<T>::Data() noexcept
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number");
    return &X;
}

template <typename T>
FloatingPointQuaternion<T> operator*(T scaleFactor, const FloatingPointQuaternion<T>& quaternion) noexcept
{
    return FloatingPointQuaternion<T>(
        scaleFactor * quaternion.X,
        scaleFactor * quaternion.Y,
        scaleFactor * quaternion.Z,
        scaleFactor * quaternion.W);
}

// explicit instantiations
template class FloatingPointQuaternion<float>;

template FloatingPointQuaternion<float>
operator*<float>(float, const FloatingPointQuaternion<float>&) noexcept;

#if defined(DBL_MANT_DIG)
template class FloatingPointQuaternion<double>;

template FloatingPointQuaternion<double>
operator*<double>(double, const FloatingPointQuaternion<double>&) noexcept;
#endif

#if defined(LDBL_MANT_DIG)
template class FloatingPointQuaternion<long double>;

template FloatingPointQuaternion<long double>
operator*<long double>(long double, const FloatingPointQuaternion<long double>&) noexcept;
#endif

} // namespace Detail
} // namespace Pomdog
