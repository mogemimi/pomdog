// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::Easings {

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

} // namespace pomdog::detail::Easings

namespace pomdog::Easings {

using EaseBack = detail::Easings::EaseBack<f32>;
using EaseBounce = detail::Easings::EaseBounce<f32>;
using EaseCircle = detail::Easings::EaseCircle<f32>;
using EaseCubic = detail::Easings::EaseCubic<f32>;
using EaseElastic = detail::Easings::EaseElastic<f32>;
using EaseExponential = detail::Easings::EaseExponential<f32>;
using EaseQuadratic = detail::Easings::EaseQuadratic<f32>;
using EaseQuartic = detail::Easings::EaseQuartic<f32>;
using EaseQuintic = detail::Easings::EaseQuintic<f32>;
using EaseSine = detail::Easings::EaseSine<f32>;

} // namespace pomdog::Easings
