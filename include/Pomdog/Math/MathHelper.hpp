// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/Degree.hpp"
#include "Pomdog/Math/Radian.hpp"
#include <type_traits>

namespace Pomdog {
namespace Detail {

template <class Tp>
struct IsTaggedFloatingPoint
: public std::integral_constant<bool, false> {};

template <class T, class Tag>
struct IsTaggedFloatingPoint<TaggedArithmetic<T, Tag>>
: public std::integral_constant<bool, std::is_floating_point<T>::value> {};

} // namespace Detail

namespace Math {

template <typename T> constexpr T Pi         = T(3.14159265358979323846L);
template <typename T> constexpr T TwoPi      = T(6.28318530717958647692L);
template <typename T> constexpr T OneOverPi  = T(0.31830988618379067154L);
template <typename T> constexpr T OneOver2Pi = T(0.15915494309189533576L);
template <typename T> constexpr T PiOver2    = T(1.57079632679489661923L);
template <typename T> constexpr T PiOver4    = T(0.78539816339744830962L);

template <> constexpr float Pi<float>         = 3.1415926535f;
template <> constexpr float TwoPi<float>      = 6.2831853071f;
template <> constexpr float OneOverPi<float>  = 0.3183098861f;
template <> constexpr float OneOver2Pi<float> = 0.1591549430f;
template <> constexpr float PiOver2<float>    = 1.5707963267f;
template <> constexpr float PiOver4<float>    = 0.7853981633f;

} // namespace Math

namespace MathHelper {

template <typename T>
T Clamp(const T& x, const T& min, const T& max)
{
    static_assert(std::is_arithmetic<T>::value
        || Detail::IsTaggedFloatingPoint<T>::value, "");
    POMDOG_ASSERT_MESSAGE(min < max, "In Clamp, maxval is out of range");
    if (x < min) {
        return min;
    }
    if (x > max) {
        return max;
    }
    return x;
}

template <typename T>
T Saturate(const T& x)
{
    static_assert(std::is_floating_point<T>::value ||
        Detail::IsTaggedFloatingPoint<T>::value,
        "T is floaing point number");
    return Clamp(x, T{0}, T{1});
}

template <typename T>
T Lerp(const T& source1, const T& source2, const T& amount) noexcept
{
    static_assert(std::is_floating_point<T>::value ||
        Detail::IsTaggedFloatingPoint<T>::value,
        "T is floaing point number");
    return source1 + amount * (source2 - source1);
}

template <typename T>
T SmoothStep(const T& min, const T& max, const T& amount)
{
    static_assert(std::is_floating_point<T>::value ||
        Detail::IsTaggedFloatingPoint<T>::value,
        "T is floaing point number");
    //POMDOG_ASSERT(amount >= 0);
    //POMDOG_ASSERT(amount <= 1);
    auto x = Saturate(amount);
    auto scale = x * x * (T{3} - T{2} * x);
    return min + scale * (max - min);
}

template <typename T>
Radian<T> ToRadians(const Degree<T>& degrees) noexcept
{
    static_assert(std::is_floating_point<T>::value, "");
    constexpr auto scaleFactor = Math::Pi<T> * (T{1} / T{180});
    return Radian<T>(degrees.value * scaleFactor);
}

template <typename T>
Radian<T> ToRadians(const T& degrees) noexcept
{
    static_assert(std::is_floating_point<T>::value, "");
    constexpr auto scaleFactor = Math::Pi<T> * (T{1} / T{180});
    return Radian<T>(degrees * scaleFactor);
}

template <typename T>
Degree<T> ToDegrees(const Radian<T>& radians) noexcept
{
    static_assert(std::is_floating_point<T>::value, "");
    constexpr auto scaleFactor = T{180} * (T{1} / Math::Pi<T>);
    return Degree<T>(radians.value * scaleFactor);
}

template <typename T>
Degree<T> ToDegrees(const T& radians) noexcept
{
    static_assert(std::is_floating_point<T>::value, "");
    constexpr auto scaleFactor = T{180} * (T{1} / Math::Pi<T>);
    return Degree<T>(radians * scaleFactor);
}

POMDOG_EXPORT Point2D ToPoint2D(const Vector2& vec) noexcept;

POMDOG_EXPORT Vector2 ToVector2(const Point2D& point) noexcept;

POMDOG_EXPORT Point3D ToPoint3D(const Vector3& vec) noexcept;

POMDOG_EXPORT Vector3 ToVector3(const Point3D& point) noexcept;

} // namespace MathHelper
} // namespace Pomdog
