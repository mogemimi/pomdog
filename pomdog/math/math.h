// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"

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

[[nodiscard]] float POMDOG_EXPORT
Clamp(float x, float min, float max) noexcept;

[[nodiscard]] double POMDOG_EXPORT
Clamp(double x, double min, double max) noexcept;

[[nodiscard]] float POMDOG_EXPORT
Saturate(float x) noexcept;

[[nodiscard]] double POMDOG_EXPORT
Saturate(double x) noexcept;

[[nodiscard]] float POMDOG_EXPORT
Lerp(float source1, float source2, float amount) noexcept;

[[nodiscard]] double POMDOG_EXPORT
Lerp(double source1, double source2, double amount) noexcept;

[[nodiscard]] float POMDOG_EXPORT
SmoothStep(float min, float max, float amount) noexcept;

[[nodiscard]] double POMDOG_EXPORT
SmoothStep(double min, double max, double amount) noexcept;

} // namespace pomdog::math
