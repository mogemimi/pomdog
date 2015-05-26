// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_MATHHELPER_F07877B3_HPP
#define POMDOG_MATHHELPER_F07877B3_HPP

#include "Degree.hpp"
#include "Radian.hpp"
#include <cstdint>
#include <cfloat>
#include <type_traits>

namespace Pomdog {

template <typename T>
struct MathConstants;

template <>
struct MathConstants<float>
{
    static constexpr float Pi()         { return 3.1415926535f; }
    static constexpr float TwoPi()      { return 6.2831853071f; }
    static constexpr float OneOverPi()  { return 0.3183098861f; }
    static constexpr float OneOver2Pi() { return 0.1591549430f; }
    static constexpr float PiOver2()    { return 1.5707963267f; }
    static constexpr float PiOver4()    { return 0.7853981633f; }
};

template <>
struct MathConstants<double>
{
    static constexpr double Pi()         { return 3.14159265358979323846; }
    static constexpr double TwoPi()      { return 6.28318530717958647692; }
    static constexpr double OneOverPi()  { return 0.31830988618379067154; }
    static constexpr double OneOver2Pi() { return 0.15915494309189533576; }
    static constexpr double PiOver2()    { return 1.57079632679489661923; }
    static constexpr double PiOver4()    { return 0.78539816339744830962; }
};

namespace MathHelper {
//-------------------------------------------------------------------
template <typename T>
T Clamp(T const& x, T const& min, T const& max)
{
    static_assert(std::is_arithmetic<T>::value, "");
    POMDOG_ASSERT_MESSAGE(min < max, "In Clamp, maxval is out of range");
    if (x < min) {
        return min;
    }
    if (x > max) {
        return max;
    }
    return x;
}
//-------------------------------------------------------------------
template <typename T>
T Saturate(T const& x)
{
    static_assert(std::is_floating_point<T>::value, "T is floaing point number");
    return Clamp(x, T{0}, T{1});
}
//-------------------------------------------------------------------
template <typename T>
T Lerp(T const& source1, T const& source2, T const& amount)
{
    static_assert(std::is_floating_point<T>::value, "T is floaing point number");
    return source1 + amount * (source2 - source1);
}
//-------------------------------------------------------------------
template <typename T>
T SmoothStep(T const& min, T const& max, T const& amount)
{
    static_assert(std::is_floating_point<T>::value, "T is floaing point number");
    //POMDOG_ASSERT(amount >= 0);
    //POMDOG_ASSERT(amount <= 1);
    auto x = Saturate(amount);
    auto scale = x * x * (T{3} - T{2} * x);
    return min + scale * (max - min);
}
//-------------------------------------------------------------------
template <typename T>
Radian<T> ToRadians(Degree<T> const& degrees)
{
    static_assert(std::is_floating_point<T>::value, "");
    constexpr auto scaleFactor = MathConstants<T>::Pi() * (T{1} / T{180});
    return Radian<T>(degrees.value * scaleFactor);
}
//-------------------------------------------------------------------
template <typename T>
Degree<T> ToDegrees(Radian<T> const& radians)
{
    static_assert(std::is_floating_point<T>::value, "");
    constexpr auto scaleFactor = T{180} * (T{1} / MathConstants<T>::Pi());
    return Degree<T>(radians.value * scaleFactor);
}

} // namespace MathHelper
} // namespace Pomdog

#endif // POMDOG_MATHHELPER_F07877B3_HPP
