// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/detail/FloatingPointVector3.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Math/detail/FloatingPointMatrix4x4.hpp"
#include "Pomdog/Math/detail/FloatingPointQuaternion.hpp"
#include "Pomdog/Math/detail/FloatingPointVector2.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <limits>
#include <utility>

namespace Pomdog {
namespace Detail {

template <typename T>
FloatingPointVector3<T>::FloatingPointVector3(T x, T y, T z) noexcept
    : X(x)
    , Y(y)
    , Z(z)
{
}

template <typename T>
FloatingPointVector3<T>::FloatingPointVector3(const FloatingPointVector2<T>& vec, T z) noexcept
    : X(vec.X)
    , Y(vec.Y)
    , Z(z)
{
}

template <typename T>
FloatingPointVector3<T> & FloatingPointVector3<T>::operator+=(const FloatingPointVector3& vec) noexcept
{
    X += vec.X;
    Y += vec.Y;
    Z += vec.Z;
    return *this;
}

template <typename T>
FloatingPointVector3<T> & FloatingPointVector3<T>::operator-=(const FloatingPointVector3& vec) noexcept
{
    X -= vec.X;
    Y -= vec.Y;
    Z -= vec.Z;
    return *this;
}

template <typename T>
FloatingPointVector3<T> & FloatingPointVector3<T>::operator*=(T scaleFactor) noexcept
{
    X *= scaleFactor;
    Y *= scaleFactor;
    Z *= scaleFactor;
    return *this;
}

template <typename T>
FloatingPointVector3<T> & FloatingPointVector3<T>::operator/=(T scaleFactor)
{
    X /= scaleFactor;
    Y /= scaleFactor;
    Z /= scaleFactor;
    return *this;
}

template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator+() const noexcept
{
    return *this;
}

template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator-() const noexcept
{
    return FloatingPointVector3(-X, -Y, -Z);
}

template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator+(const FloatingPointVector3& other) const noexcept
{
    return FloatingPointVector3(
        X + other.X,
        Y + other.Y,
        Z + other.Z);
}

template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator-(const FloatingPointVector3& other) const noexcept
{
    return FloatingPointVector3(
        X - other.X,
        Y - other.Y,
        Z - other.Z);
}

template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator*(const FloatingPointVector3& other) const noexcept
{
    return FloatingPointVector3(
        X * other.X,
        Y * other.Y,
        Z * other.Z);
}

template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator/(const FloatingPointVector3& other) const
{
    POMDOG_ASSERT(other.X != 0);
    POMDOG_ASSERT(other.Y != 0);
    POMDOG_ASSERT(other.Z != 0);

    return FloatingPointVector3(
        X / other.X,
        Y / other.Y,
        Z / other.Z);
}

template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator*(T scaleFactor) const noexcept
{
    return FloatingPointVector3(
        X * scaleFactor,
        Y * scaleFactor,
        Z * scaleFactor);
}

template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator/(T scaleFactor) const
{
    POMDOG_ASSERT(scaleFactor != 0);
    //static_assert(!std::is_same<decltype(scaleFactor), ChuckNorris>::value,
    //  "NOTE: Chuck Norris can divide by zero.");
    return FloatingPointVector3(
        X / scaleFactor,
        Y / scaleFactor,
        Z / scaleFactor);
}

template <typename T>
bool FloatingPointVector3<T>::operator==(const FloatingPointVector3& other) const noexcept
{
    return X == other.X && Y == other.Y && Z == other.Z;
}

template <typename T>
bool FloatingPointVector3<T>::operator!=(const FloatingPointVector3& other) const noexcept
{
    return X != other.X || Y != other.Y || Z != other.Z;
}

template <typename T>
T FloatingPointVector3<T>::Length() const noexcept
{
    return std::sqrt(X * X + Y * Y + Z * Z);
}

template <typename T>
T FloatingPointVector3<T>::LengthSquared() const noexcept
{
    return X * X + Y * Y + Z * Z;
}

template <typename T>
T FloatingPointVector3<T>::Distance(const FloatingPointVector3& a, const FloatingPointVector3& b) noexcept
{
    return (a - b).Length();
}

template <typename T>
T FloatingPointVector3<T>::DistanceSquared(const FloatingPointVector3& a, const FloatingPointVector3& b) noexcept
{
    return (a - b).LengthSquared();
}

template <typename T>
T FloatingPointVector3<T>::Dot(const FloatingPointVector3& a, const FloatingPointVector3& b) noexcept
{
    return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
}

template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::Cross(const FloatingPointVector3& a, const FloatingPointVector3& b) noexcept
{
    return FloatingPointVector3(
        a.Y * b.Z - a.Z * b.Y,
        a.Z * b.X - a.X * b.Z,
        a.X * b.Y - a.Y * b.X);
}

template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::Min(const FloatingPointVector3& a, const FloatingPointVector3& b) noexcept
{
    return FloatingPointVector3(
        std::min(a.X, b.X),
        std::min(a.Y, b.Y),
        std::min(a.Z, b.Z));
}

template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::Max(const FloatingPointVector3& a, const FloatingPointVector3& b) noexcept
{
    return FloatingPointVector3(
        std::max(a.X, b.X),
        std::max(a.Y, b.Y),
        std::max(a.Z, b.Z));
}

template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::Clamp(const FloatingPointVector3& source, const FloatingPointVector3& min, const FloatingPointVector3& max) noexcept
{
    return FloatingPointVector3(
        MathHelper::Clamp(source.X, min.X, max.X),
        MathHelper::Clamp(source.Y, min.Y, max.Y),
        MathHelper::Clamp(source.Z, min.Z, max.Z));
}

template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::Lerp(const FloatingPointVector3& source1, const FloatingPointVector3& source2, T amount)
{
    return FloatingPointVector3(
        MathHelper::Lerp(source1.X, source2.X, amount),
        MathHelper::Lerp(source1.Y, source2.Y, amount),
        MathHelper::Lerp(source1.Z, source2.Z, amount));
}

template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::SmoothStep(const FloatingPointVector3& source1, const FloatingPointVector3& source2, T amount)
{
    return FloatingPointVector3(
        MathHelper::SmoothStep(source1.X, source2.X, amount),
        MathHelper::SmoothStep(source1.Y, source2.Y, amount),
        MathHelper::SmoothStep(source1.Z, source2.Z, amount));
}

template <typename T>
void
FloatingPointVector3<T>::Normalize() noexcept
{
    *this = Normalize(*this);
}

template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::Normalize(const FloatingPointVector3& source) noexcept
{
    FloatingPointVector3 result;
    Normalize(source, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointVector3<T>::Normalize(const FloatingPointVector3& source, FloatingPointVector3 & result) noexcept
{
    auto const length = source.Length();

    if (length > std::numeric_limits<decltype(length)>::epsilon()) {
        constexpr T One = 1;
        auto const inverseLength = One / length;
        result.X = source.X * inverseLength;
        result.Y = source.Y * inverseLength;
        result.Z = source.Z * inverseLength;
    }
}

template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::Transform(const FloatingPointVector3& position, const FloatingPointMatrix4x4<T>& matrix) noexcept
{
    return FloatingPointVector3(
        (position.X * matrix.m[0][0]) + (position.Y * matrix.m[1][0]) + (position.Z * matrix.m[2][0]) + matrix.m[3][0],
        (position.X * matrix.m[0][1]) + (position.Y * matrix.m[1][1]) + (position.Z * matrix.m[2][1]) + matrix.m[3][1],
        (position.X * matrix.m[0][2]) + (position.Y * matrix.m[1][2]) + (position.Z * matrix.m[2][2]) + matrix.m[3][2]);
}

template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::TransformNormal(const FloatingPointVector3& normal, const FloatingPointMatrix4x4<T>& matrix) noexcept
{
    return FloatingPointVector3(
        (normal.X * matrix.m[0][0]) + (normal.Y * matrix.m[1][0]) + (normal.Z * matrix.m[2][0]),
        (normal.X * matrix.m[0][1]) + (normal.Y * matrix.m[1][1]) + (normal.Z * matrix.m[2][1]),
        (normal.X * matrix.m[0][2]) + (normal.Y * matrix.m[1][2]) + (normal.Z * matrix.m[2][2]));
}

template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::ToEulerAngles(const FloatingPointQuaternion<T>& q)
{
    constexpr T Two = 2;
    const auto xx = q.X * q.X;
    const auto yy = q.Y * q.Y;
    const auto zz = q.Z * q.Z;
    const auto ww = q.W * q.W;
    auto roll = std::atan2(Two * (q.W * q.Z + q.X * q.Y), ww + xx - yy - zz);
    auto yaw = std::asin(Two * (q.W * q.Y - q.Z * q.X));
    auto pitch = std::atan2(Two * (q.W * q.X + q.Y * q.Z), ww - xx - yy + zz);
    return FloatingPointVector3(pitch, yaw, roll);
}

template <typename T>
const T* FloatingPointVector3<T>::Data() const noexcept
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number");
    return &X;
}

template <typename T>
T* FloatingPointVector3<T>::Data() noexcept
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number");
    return &X;
}

template <typename T>
FloatingPointVector3<T> operator*(T scaleFactor, const FloatingPointVector3<T>& vector) noexcept
{
    return FloatingPointVector3<T>(
        scaleFactor * vector.X,
        scaleFactor * vector.Y,
        scaleFactor * vector.Z);
}

// explicit instantiations
template class FloatingPointVector3<float>;

template FloatingPointVector3<float>
operator*<float>(float, const FloatingPointVector3<float>&) noexcept;

#if defined(DBL_MANT_DIG)
template class FloatingPointVector3<double>;

template FloatingPointVector3<double>
operator*<double>(double, const FloatingPointVector3<double>&) noexcept;
#endif

#if defined(LDBL_MANT_DIG)
template class FloatingPointVector3<long double>;

template FloatingPointVector3<long double>
operator*<long double>(long double, const FloatingPointVector3<long double>&) noexcept;
#endif

} // namespace Detail
} // namespace Pomdog
