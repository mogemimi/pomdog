// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::math {

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
constexpr inline float Pi<f32> = 3.1415926535f;
template <>
constexpr inline float TwoPi<f32> = 6.2831853071f;
template <>
constexpr inline float OneOverPi<f32> = 0.3183098861f;
template <>
constexpr inline float OneOver2Pi<f32> = 0.1591549430f;
template <>
constexpr inline float PiOver2<f32> = 1.5707963267f;
template <>
constexpr inline float PiOver4<f32> = 0.7853981633f;

} // namespace pomdog::math
