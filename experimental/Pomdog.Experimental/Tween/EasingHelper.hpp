// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EASINGHELPER_2D00DDAA_HPP
#define POMDOG_EASINGHELPER_2D00DDAA_HPP

#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog.Experimental/MSVCSupport.hpp"
#include <type_traits>
#include <cmath>

namespace Pomdog {
namespace Detail {
namespace Easings {

template <typename T, T(*Function)(T)>
class Ease {
public:
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    static T In(T normalizedTime)
    {
        return Function(normalizedTime);
    }

    static T Out(T normalizedTime)
    {
        return 1 - Function(1 - normalizedTime);
    }

    static T InOut(T normalizedTime)
    {
        constexpr auto half = T(1) / 2;
        return (time < half) ? half * In(normalizedTime * 2)
            : half + half * Out(normalizedTime * 2 - 1);
    }
};

template <typename T>
constexpr T Quadratic(T time)
{
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    // Quadratic easing
    return time * time;
}

template <typename T>
constexpr T Cubic(T time)
{
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    // Cubic easing
    return time * time * time;
}

template <typename T>
constexpr T Quartic(T time)
{
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    // Quartic easing
    return time * time * time * time;
}

template <typename T>
constexpr T Quintic(T time)
{
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    // Quintic easing
    return time * time * time * time * time;
}

template <typename T>
T Sine(T time)
{
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    // Sinusoidal easing
    return 1 - std::cos(time * MathConstants<T>::PiOver2());
}

template <typename T>
T Exponential(T time)
{
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    // Exponential easing
    return std::pow(2, 10 * (time - 1));
}

template <typename T>
T Circle(T time)
{
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    // Circular easing
    return 1 - std::sqrt(1 - time * time);
}

template <typename T>
T Elastic(T time)
{
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    // Elastic easing
    constexpr auto period = T(0.3);
    constexpr auto s = period / 4;
    const auto postFix = std::pow(2, 10 * (time - 1));
    return (time <= 0 || time >= 1) ? time
        : - (postFix * std::sin(((time - 1) - s) * MathConstants<T>::TwoPi() / period));
}

template <typename T>
T Bounce(T time)
{
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    // Bounce easing
    time = 1 - time;

    if (time < 1/2.75) {
        return 1 - (7.5625 * time * time);
    }
    else if (time < 2/2.75) {
        auto t = time - 1.5/2.75;
        return 1 - (7.5625 * t * t + 0.75);
    }
    else if (time < 2.5/2.75) {
        auto t = time - 2.25/2.75;
        return 1 - (7.5625 * t * t + 0.9375);
    }

    auto postFix = time -= 2.625/2.75;
    return 1 - (7.5625 * postFix * time + 0.984375);
}

template <typename T>
T Back(T time)
{
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    // Back easing
    constexpr auto s = T(1.70158);
    return time * time * ((s + 1) * time - s);
}

template <typename T>
using EaseBack = Ease<T, Back<T>>;

template <typename T>
using EaseBounce = Ease<T, Bounce<T>>;

template <typename T>
using EaseCircle = Ease<T, Circle<T>>;

template <typename T>
using EaseCubic = Ease<T, Cubic<T>>;

template <typename T>
using EaseElastic = Ease<T, Elastic<T>>;

template <typename T>
using EaseExponential = Ease<T, Exponential<T>>;

template <typename T>
using EaseQuadratic = Ease<T, Quadratic<T>>;

template <typename T>
using EaseQuartic = Ease<T, Quartic<T>>;

template <typename T>
using EaseQuintic = Ease<T, Quintic<T>>;

template <typename T>
using EaseSine = Ease<T, Sine<T>>;

}// namespace Easings
}// namespace Detail

namespace Easings {

#ifndef POMDOG_MSVC2015_SUPPORT
using EaseBack = Detail::Easings::EaseBack<float>;
using EaseBounce = Detail::Easings::EaseBounce<float>;
using EaseCircle = Detail::Easings::EaseCircle<float>;
using EaseCubic = Detail::Easings::EaseCubic<float>;
using EaseElastic = Detail::Easings::EaseElastic<float>;
using EaseExponential = Detail::Easings::EaseExponential<float>;
using EaseQuadratic = Detail::Easings::EaseQuadratic<float>;
using EaseQuartic = Detail::Easings::EaseQuartic<float>;
using EaseQuintic = Detail::Easings::EaseQuintic<float>;
using EaseSine = Detail::Easings::EaseSine<float>;
#endif

}// namespace Easings
}// namespace Pomdog

#endif // POMDOG_EASINGHELPER_2D00DDAA_HPP
