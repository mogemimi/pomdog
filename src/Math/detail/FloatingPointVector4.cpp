// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/detail/FloatingPointVector4.hpp"
#include "Pomdog/Math/detail/FloatingPointMatrix4x4.hpp"
#include "Pomdog/Math/detail/FloatingPointVector3.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <limits>
#include <utility>

namespace Pomdog {
namespace Detail {

template <typename T>
FloatingPointVector4<T>::FloatingPointVector4(const FloatingPointVector3<T>& xyz, T w) noexcept
    : X(xyz.X)
    , Y(xyz.Y)
    , Z(xyz.Z)
    , W(w)
{
}

template <typename T>
FloatingPointVector4<T>::FloatingPointVector4(T x, T y, T z, T w) noexcept
    : X(x)
    , Y(y)
    , Z(z)
    , W(w)
{
}

template <typename T>
FloatingPointVector4<T> & FloatingPointVector4<T>::operator+=(const FloatingPointVector4& other) noexcept
{
    X += other.X;
    Y += other.Y;
    Z += other.Z;
    W += other.W;
    return *this;
}

template <typename T>
FloatingPointVector4<T> & FloatingPointVector4<T>::operator-=(const FloatingPointVector4& other) noexcept
{
    X -= other.X;
    Y -= other.Y;
    Z -= other.Z;
    W -= other.W;
    return *this;
}

template <typename T>
FloatingPointVector4<T> & FloatingPointVector4<T>::operator*=(T scaleFactor) noexcept
{
    X *= scaleFactor;
    Y *= scaleFactor;
    Z *= scaleFactor;
    W *= scaleFactor;
    return *this;
}

template <typename T>
FloatingPointVector4<T> & FloatingPointVector4<T>::operator/=(T scaleFactor)
{
    X /= scaleFactor;
    Y /= scaleFactor;
    Z /= scaleFactor;
    W /= scaleFactor;
    return *this;
}

template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator+() const noexcept
{
    return *this;
}

template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator-() const noexcept
{
    return FloatingPointVector4(-X, -Y, -Z, -W);
}

template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator+(const FloatingPointVector4& other) const noexcept
{
    return FloatingPointVector4(
        X + other.X,
        Y + other.Y,
        Z + other.Z,
        W + other.W);
}

template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator-(const FloatingPointVector4& other) const noexcept
{
    return FloatingPointVector4(
        X - other.X,
        Y - other.Y,
        Z - other.Z,
        W - other.W);
}

template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator*(const FloatingPointVector4& other) const noexcept
{
    return FloatingPointVector4(
        X * other.X,
        Y * other.Y,
        Z * other.Z,
        W * other.W);
}

template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator/(const FloatingPointVector4& other) const
{
    POMDOG_ASSERT(other != FloatingPointVector4<T>(0, 0, 0, 0));
    return FloatingPointVector4(
        X / other.X,
        Y / other.Y,
        Z / other.Z,
        W / other.W);
}

template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator*(T scaleFactor) const noexcept
{
    return FloatingPointVector4(
        X * scaleFactor,
        Y * scaleFactor,
        Z * scaleFactor,
        W * scaleFactor);
}

template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator/(T scaleFactor) const
{
    POMDOG_ASSERT(scaleFactor != 0);
    //static_assert(!std::is_same<decltype(scaleFactor), ChuckNorris>::value,
    //  "NOTE: Chuck Norris can divide by zero.");
    return FloatingPointVector4(
        X / scaleFactor,
        Y / scaleFactor,
        Z / scaleFactor,
        W / scaleFactor);
}

template <typename T>
bool FloatingPointVector4<T>::operator==(const FloatingPointVector4& other) const noexcept
{
    return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
}

template <typename T>
bool FloatingPointVector4<T>::operator!=(const FloatingPointVector4& other) const noexcept
{
    return X != other.X || Y != other.Y || Z != other.Z || W != other.W;
}

template <typename T>
T FloatingPointVector4<T>::Length() const noexcept
{
    return std::sqrt(X * X + Y * Y + Z * Z + W * W);
}

template <typename T>
T FloatingPointVector4<T>::LengthSquared() const noexcept
{
    return X * X + Y * Y + Z * Z + W * W;
}

template <typename T>
T FloatingPointVector4<T>::Distance(const FloatingPointVector4& a, const FloatingPointVector4& b) noexcept
{
    return (a - b).Length();
}

template <typename T>
T FloatingPointVector4<T>::DistanceSquared(const FloatingPointVector4& a, const FloatingPointVector4& b) noexcept
{
    return (a - b).LengthSquared();
}

template <typename T>
T FloatingPointVector4<T>::Dot(const FloatingPointVector4& a, const FloatingPointVector4& b) noexcept
{
    return a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W;
}

template <typename T>
void FloatingPointVector4<T>::Normalize() noexcept
{
    *this = Normalize(*this);
}

template <typename T>
FloatingPointVector4<T>
FloatingPointVector4<T>::Normalize(const FloatingPointVector4& source) noexcept
{
    FloatingPointVector4 result;
    Normalize(source, result);
    return std::move(result);
}

template <typename T>
void FloatingPointVector4<T>::Normalize(const FloatingPointVector4& source, FloatingPointVector4 & result) noexcept
{
    auto const length = source.Length();

    if (length > std::numeric_limits<decltype(length)>::epsilon()) {
        constexpr T One = 1;
        auto const inverseLength = One / length;
        result.X = source.X * inverseLength;
        result.Y = source.Y * inverseLength;
        result.Z = source.Z * inverseLength;
        result.W = source.W * inverseLength;
    }
}

template <typename T>
FloatingPointVector4<T>
FloatingPointVector4<T>::Transform(const FloatingPointVector4& vector, const FloatingPointMatrix4x4<T>& matrix) noexcept
{
    return FloatingPointVector4{
        (vector.X * matrix.m[0][0]) + (vector.Y * matrix.m[1][0]) + (vector.Z * matrix.m[2][0]) + (vector.W * matrix.m[3][0]),
        (vector.X * matrix.m[0][1]) + (vector.Y * matrix.m[1][1]) + (vector.Z * matrix.m[2][1]) + (vector.W * matrix.m[3][1]),
        (vector.X * matrix.m[0][2]) + (vector.Y * matrix.m[1][2]) + (vector.Z * matrix.m[2][2]) + (vector.W * matrix.m[3][2]),
        (vector.X * matrix.m[0][3]) + (vector.Y * matrix.m[1][3]) + (vector.Z * matrix.m[2][3]) + (vector.W * matrix.m[3][3])
    };
}

template <typename T>
const T* FloatingPointVector4<T>::Data() const noexcept
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number");
    return &X;
}

template <typename T>
T* FloatingPointVector4<T>::Data() noexcept
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number");
    return &X;
}

template <typename T>
FloatingPointVector4<T> operator*(T scaleFactor, const FloatingPointVector4<T>& vector) noexcept
{
    return FloatingPointVector4<T>(
        scaleFactor * vector.X,
        scaleFactor * vector.Y,
        scaleFactor * vector.Z,
        scaleFactor * vector.W);
}

// explicit instantiations
template class FloatingPointVector4<float>;

template FloatingPointVector4<float>
operator*<float>(float, const FloatingPointVector4<float>&) noexcept;

#if defined(DBL_MANT_DIG)
template class FloatingPointVector4<double>;

template FloatingPointVector4<double>
operator*<double>(double, const FloatingPointVector4<double>&) noexcept;
#endif

#if defined(LDBL_MANT_DIG)
template class FloatingPointVector4<long double>;
template FloatingPointVector4<long double>
operator*<long double>(long double, const FloatingPointVector4<long double>&) noexcept;
#endif

} // namespace Detail
} // namespace Pomdog
