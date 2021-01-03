// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/Degree.hpp"
#include "Pomdog/Math/Detail/TypeTraits.hpp"
#include "Pomdog/Math/Radian.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Math {

template <typename T>
inline constexpr T Pi = T(3.14159265358979323846L);
template <typename T>
inline constexpr T TwoPi = T(6.28318530717958647692L);
template <typename T>
inline constexpr T OneOverPi = T(0.31830988618379067154L);
template <typename T>
inline constexpr T OneOver2Pi = T(0.15915494309189533576L);
template <typename T>
inline constexpr T PiOver2 = T(1.57079632679489661923L);
template <typename T>
inline constexpr T PiOver4 = T(0.78539816339744830962L);

template <>
constexpr inline float Pi<float> = 3.1415926535f;
template <>
constexpr inline float TwoPi<float> = 6.2831853071f;
template <>
constexpr inline float OneOverPi<float> = 0.3183098861f;
template <>
constexpr inline float OneOver2Pi<float> = 0.1591549430f;
template <>
constexpr inline float PiOver2<float> = 1.5707963267f;
template <>
constexpr inline float PiOver4<float> = 0.7853981633f;

template <typename T>
[[nodiscard]] T
Clamp(T x, T min, T max) noexcept
{
    static_assert(std::is_arithmetic<T>::value || Detail::IsTaggedFloatingPoint<T>::value, "");
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
[[nodiscard]] T
Saturate(T x) noexcept
{
    static_assert(
        std::is_floating_point<T>::value || Detail::IsTaggedFloatingPoint<T>::value,
        "T is floaing point number");
    return Clamp(x, T{0}, T{1});
}

template <typename T>
[[nodiscard]] T
Lerp(T source1, T source2, T amount) noexcept
{
    static_assert(
        std::is_floating_point<T>::value || Detail::IsTaggedFloatingPoint<T>::value,
        "T is floaing point number");
    return source1 + amount * (source2 - source1);
}

template <typename T>
[[nodiscard]] T
SmoothStep(T min, T max, T amount) noexcept
{
    static_assert(
        std::is_floating_point<T>::value || Detail::IsTaggedFloatingPoint<T>::value,
        "T is floaing point number");
    const auto x = Saturate(amount);
    const auto scale = x * x * (T{3} - T{2} * x);
    return min + scale * (max - min);
}

template <typename T>
[[nodiscard]] Radian<T>
ToRadians(const Degree<T>& degrees) noexcept
{
    static_assert(std::is_floating_point<T>::value, "");
    constexpr auto scaleFactor = Math::Pi<T> * (T{1} / T{180});
    return Radian<T>(degrees.value * scaleFactor);
}

template <typename T>
[[nodiscard]] Radian<T>
ToRadians(const T& degrees) noexcept
{
    static_assert(std::is_floating_point<T>::value, "");
    constexpr auto scaleFactor = Math::Pi<T> * (T{1} / T{180});
    return Radian<T>(degrees * scaleFactor);
}

template <typename T>
[[nodiscard]] Degree<T>
ToDegrees(const Radian<T>& radians) noexcept
{
    static_assert(std::is_floating_point<T>::value, "");
    constexpr auto scaleFactor = T{180} * (T{1} / Math::Pi<T>);
    return Degree<T>(radians.value * scaleFactor);
}

template <typename T>
[[nodiscard]] Degree<T>
ToDegrees(const T& radians) noexcept
{
    static_assert(std::is_floating_point<T>::value, "");
    constexpr auto scaleFactor = T{180} * (T{1} / Math::Pi<T>);
    return Degree<T>(radians * scaleFactor);
}

[[nodiscard]] POMDOG_EXPORT Point2D
ToPoint2D(const Vector2& vec) noexcept;

[[nodiscard]] POMDOG_EXPORT Vector2
ToVector2(const Point2D& point) noexcept;

[[nodiscard]] POMDOG_EXPORT Point3D
ToPoint3D(const Vector3& vec) noexcept;

[[nodiscard]] POMDOG_EXPORT Vector3
ToVector3(const Point3D& point) noexcept;

[[nodiscard]] POMDOG_EXPORT Point2D
Abs(const Point2D& point) noexcept;

[[nodiscard]] POMDOG_EXPORT Point3D
Abs(const Point3D& point) noexcept;

} // namespace Pomdog::Math
