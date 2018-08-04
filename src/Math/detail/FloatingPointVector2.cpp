// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/detail/FloatingPointVector2.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Math/detail/FloatingPointMatrix3x2.hpp"
#include "Pomdog/Math/detail/FloatingPointMatrix4x4.hpp"
#include "Pomdog/Math/detail/FloatingPointQuaternion.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <limits>

namespace Pomdog {
namespace Detail {

template <typename T>
FloatingPointVector2<T>::FloatingPointVector2(T x, T y) noexcept
    : X(x)
    , Y(y)
{
}

template <typename T>
FloatingPointVector2<T>& FloatingPointVector2<T>::operator+=(const FloatingPointVector2& other) noexcept
{
    this->X += other.X;
    this->Y += other.Y;
    return *this;
}

template <typename T>
FloatingPointVector2<T>& FloatingPointVector2<T>::operator-=(const FloatingPointVector2& other) noexcept
{
    this->X -= other.X;
    this->Y -= other.Y;
    return *this;
}

template <typename T>
FloatingPointVector2<T>& FloatingPointVector2<T>::operator*=(T scaleFactor) noexcept
{
    this->X *= scaleFactor;
    this->Y *= scaleFactor;
    return *this;
}

template <typename T>
FloatingPointVector2<T>& FloatingPointVector2<T>::operator/=(T scaleFactor)
{
    this->X /= scaleFactor;
    this->Y /= scaleFactor;
    return *this;
}

template <typename T>
FloatingPointVector2<T> FloatingPointVector2<T>::operator+() const noexcept
{
    return *this;
}

template <typename T>
FloatingPointVector2<T> FloatingPointVector2<T>::operator-() const noexcept
{
    return FloatingPointVector2(-X, -Y);
}

template <typename T>
FloatingPointVector2<T> FloatingPointVector2<T>::operator*(T scaleFactor) const noexcept
{
    return FloatingPointVector2(X * scaleFactor, Y * scaleFactor);
}

template <typename T>
FloatingPointVector2<T> FloatingPointVector2<T>::operator/(T scaleFactor) const
{
    POMDOG_ASSERT(scaleFactor != 0);
    //static_assert(!std::is_same<decltype(scaleFactor), ChuckNorris>::value,
    //  "NOTE: Chuck Norris can divide by zero.");
    return FloatingPointVector2(X / scaleFactor, Y / scaleFactor);
}

template <typename T>
FloatingPointVector2<T> FloatingPointVector2<T>::operator+(const FloatingPointVector2& other) const noexcept
{
    return FloatingPointVector2(X + other.X, Y + other.Y);
}

template <typename T>
FloatingPointVector2<T> FloatingPointVector2<T>::operator-(const FloatingPointVector2& other) const noexcept
{
    return FloatingPointVector2(X - other.X, Y - other.Y);
}

template <typename T>
FloatingPointVector2<T> FloatingPointVector2<T>::operator*(const FloatingPointVector2& other) const noexcept
{
    return FloatingPointVector2(X * other.X, Y * other.Y);
}

template <typename T>
FloatingPointVector2<T> FloatingPointVector2<T>::operator/(const FloatingPointVector2& other) const
{
    return FloatingPointVector2(X / other.X, Y / other.Y);
}

template <typename T>
bool FloatingPointVector2<T>::operator==(const FloatingPointVector2& other) const noexcept
{
    return X == other.X && Y == other.Y;
}

template <typename T>
bool FloatingPointVector2<T>::operator!=(const FloatingPointVector2& other) const noexcept
{
    return X != other.X || Y != other.Y;
}

template <typename T>
T FloatingPointVector2<T>::Length() const noexcept
{
    return std::sqrt(X * X + Y * Y);
}

template <typename T>
T FloatingPointVector2<T>::LengthSquared() const noexcept
{
    return X * X + Y * Y;
}

template <typename T>
T FloatingPointVector2<T>::Distance(const FloatingPointVector2& a, const FloatingPointVector2& b) noexcept
{
    return (a - b).Length();
}

template <typename T>
T FloatingPointVector2<T>::DistanceSquared(const FloatingPointVector2& a, const FloatingPointVector2& b) noexcept
{
    return (a - b).LengthSquared();
}

template <typename T>
T FloatingPointVector2<T>::Dot(const FloatingPointVector2& a, const FloatingPointVector2& b) noexcept
{
    return a.X * b.X + a.Y * b.Y;
}

template <typename T>
T FloatingPointVector2<T>::Cross(const FloatingPointVector2& a, const FloatingPointVector2& b) noexcept
{
    return a.X * b.Y - a.Y * b.X;
}

template <typename T>
FloatingPointVector2<T>
FloatingPointVector2<T>::Min(const FloatingPointVector2& a, const FloatingPointVector2& b) noexcept
{
    return FloatingPointVector2{std::min(a.X, b.X), std::min(a.Y, b.Y)};
}

template <typename T>
FloatingPointVector2<T>
FloatingPointVector2<T>::Max(const FloatingPointVector2& a, const FloatingPointVector2& b) noexcept
{
    return FloatingPointVector2{std::max(a.X, b.X), std::max(a.Y, b.Y)};
}

template <typename T>
FloatingPointVector2<T>
FloatingPointVector2<T>::Clamp(const FloatingPointVector2& source, const FloatingPointVector2& min, const FloatingPointVector2& max) noexcept
{
    return FloatingPointVector2{
        MathHelper::Clamp(source.X, min.X, max.X),
        MathHelper::Clamp(source.Y, min.Y, max.Y)};
}

template <typename T>
FloatingPointVector2<T>
FloatingPointVector2<T>::Lerp(const FloatingPointVector2& source1, const FloatingPointVector2& source2, T amount)
{
    return FloatingPointVector2(
        MathHelper::Lerp(source1.X, source2.X, amount),
        MathHelper::Lerp(source1.Y, source2.Y, amount));
}

template <typename T>
FloatingPointVector2<T>
FloatingPointVector2<T>::SmoothStep(const FloatingPointVector2& source1, const FloatingPointVector2& source2, T amount)
{
    return FloatingPointVector2(
        MathHelper::SmoothStep(source1.X, source2.X, amount),
        MathHelper::SmoothStep(source1.Y, source2.Y, amount));
}

template <typename T>
void FloatingPointVector2<T>::Normalize() noexcept
{
    *this = Normalize(*this);
}

template <typename T>
FloatingPointVector2<T>
FloatingPointVector2<T>::Normalize(const FloatingPointVector2& source) noexcept
{
    FloatingPointVector2 result;
    Normalize(source, result);
    return result;
}

template <typename T>
void
FloatingPointVector2<T>::Normalize(const FloatingPointVector2& source, FloatingPointVector2 & result) noexcept
{
    auto const length = source.Length();

    if (length > std::numeric_limits<decltype(length)>::epsilon()) {
        constexpr T One = 1;
        auto const InverseLength = One / length;
        result.X = source.X * InverseLength;
        result.Y = source.Y * InverseLength;
        return;
    }
}

template <typename T>
FloatingPointVector2<T>
FloatingPointVector2<T>::Transform(const FloatingPointVector2& position, const FloatingPointMatrix3x2<T>& matrix) noexcept
{
    return FloatingPointVector2(
        (position.X * matrix.m[0][0]) + (position.Y * matrix.m[1][0]) + matrix.m[2][0],
        (position.X * matrix.m[0][1]) + (position.Y * matrix.m[1][1]) + matrix.m[2][1]);
}

template <typename T>
FloatingPointVector2<T>
FloatingPointVector2<T>::Transform(const FloatingPointVector2& position, const FloatingPointMatrix4x4<T>& matrix) noexcept
{
    return FloatingPointVector2(
        (position.X * matrix.m[0][0]) + (position.Y * matrix.m[1][0]) + matrix.m[3][0],
        (position.X * matrix.m[0][1]) + (position.Y * matrix.m[1][1]) + matrix.m[3][1]);
}

template <typename T>
FloatingPointVector2<T>
FloatingPointVector2<T>::Transform(const FloatingPointVector2& position, const FloatingPointQuaternion<T>& rotation) noexcept
{
    auto const x = 2 * (position.Y * -rotation.Z);
    auto const y = 2 * (position.X * rotation.Z);
    auto const z = 2 * (position.Y * rotation.X - position.X * rotation.Y);

    return FloatingPointVector2(
        position.X + x * rotation.W + (rotation.Y * z - rotation.Z * y),
        position.Y + y * rotation.W + (rotation.Z * x - rotation.X * z));
}

template <typename T>
FloatingPointVector2<T>
FloatingPointVector2<T>::TransformNormal(const FloatingPointVector2& normal, const FloatingPointMatrix4x4<T>& matrix) noexcept
{
    return FloatingPointVector2(
        (normal.X * matrix.m[0][0]) + (normal.Y * matrix.m[1][0]),
        (normal.X * matrix.m[0][1]) + (normal.Y * matrix.m[1][1]));
}

template <typename T>
const T* FloatingPointVector2<T>::Data() const noexcept
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number");
    return &X;
}

template <typename T>
T* FloatingPointVector2<T>::Data() noexcept
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number");
    return &X;
}

template <typename T>
FloatingPointVector2<T> operator*(T scaleFactor, const FloatingPointVector2<T>& vector) noexcept
{
    return FloatingPointVector2<T>(scaleFactor * vector.X, scaleFactor * vector.Y);
}

// explicit instantiations
template class FloatingPointVector2<float>;

template FloatingPointVector2<float>
operator*<float>(float, const FloatingPointVector2<float>&) noexcept;

#if defined(DBL_MANT_DIG)
template class FloatingPointVector2<double>;

template FloatingPointVector2<double>
operator*<double>(double, const FloatingPointVector2<double>&) noexcept;
#endif

#if defined(LDBL_MANT_DIG)
template class FloatingPointVector2<long double>;

template FloatingPointVector2<long double>
operator*<long double>(long double, const FloatingPointVector2<long double>&) noexcept;
#endif

} // namespace Detail
} // namespace Pomdog
