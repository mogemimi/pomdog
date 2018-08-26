// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cmath>
#include <type_traits>

namespace Pomdog {
namespace Detail {
namespace Easings {

template <typename T, T (*Function)(T)>
class POMDOG_EXPORT Ease final {
public:
    static_assert(Function != nullptr);
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    static T In(T normalizedTime)
    {
        return Function(normalizedTime);
    }

    static T Out(T normalizedTime)
    {
        return static_cast<T>(1) - Function(static_cast<T>(1) - normalizedTime);
    }

    static T InOut(T normalizedTime)
    {
        constexpr auto half = static_cast<T>(1) / static_cast<T>(2);
        return (normalizedTime < half)
            ? half * In(normalizedTime * static_cast<T>(2))
            : half + half * Out(normalizedTime * static_cast<T>(2) - static_cast<T>(1));
    }
};

template <typename T>
T POMDOG_EXPORT Quadratic(T time) noexcept;

template <typename T>
T POMDOG_EXPORT Cubic(T time) noexcept;

template <typename T>
T POMDOG_EXPORT Quartic(T time) noexcept;

template <typename T>
T POMDOG_EXPORT Quintic(T time) noexcept;

template <typename T>
T POMDOG_EXPORT Sine(T time) noexcept;

template <typename T>
T POMDOG_EXPORT Exponential(T time) noexcept;

template <typename T>
T POMDOG_EXPORT Circle(T time) noexcept;

template <typename T>
T POMDOG_EXPORT Elastic(T time) noexcept;

template <typename T>
T POMDOG_EXPORT Bounce(T time) noexcept;

template <typename T>
T POMDOG_EXPORT Back(T time) noexcept;

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

} // namespace Easings
} // namespace Detail

namespace Easings {

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

} // namespace Easings
} // namespace Pomdog
