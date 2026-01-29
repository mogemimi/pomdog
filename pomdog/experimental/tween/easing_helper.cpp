// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/tween/easing_helper.h"
#include "pomdog/math/math_constants.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::Easings {

template <typename T>
T Quadratic(T time) noexcept
{
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    // Quadratic easing
    return time * time;
}

template <typename T>
T Cubic(T time) noexcept
{
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    // Cubic easing
    return time * time * time;
}

template <typename T>
T Quartic(T time) noexcept
{
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    // Quartic easing
    return time * time * time * time;
}

template <typename T>
T Quintic(T time) noexcept
{
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    // Quintic easing
    return time * time * time * time * time;
}

template <typename T>
T Sine(T time) noexcept
{
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    // Sinusoidal easing
    return 1 - std::cos(time * math::PiOver2<T>);
}

template <typename T>
T Exponential(T time) noexcept
{
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    // Exponential easing
    return std::pow(T(2), 10 * (time - 1));
}

template <typename T>
T Circle(T time) noexcept
{
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    // Circular easing
    return 1 - std::sqrt(1 - time * time);
}

template <typename T>
T Elastic(T time) noexcept
{
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    // Elastic easing
    constexpr auto period = T(0.3);
    constexpr auto s = period / 4;
    const auto postFix = std::pow(T(2), 10 * (time - 1));
    return (time <= 0 || time >= 1)
               ? time
               : -(postFix * std::sin(((time - 1) - s) * math::TwoPi<T> / period));
}

template <typename T>
T Bounce(T time) noexcept
{
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    // Bounce easing
    time = 1 - time;

    if (time < T(1 / 2.75)) {
        return 1 - (T(7.5625) * time * time);
    }
    else if (time < T(2 / 2.75)) {
        auto t = time - T(1.5 / 2.75);
        return 1 - (T(7.5625) * t * t + T(0.75));
    }
    else if (time < T(2.5 / 2.75)) {
        auto t = time - T(2.25 / 2.75);
        return 1 - (T(7.5625) * t * t + T(0.9375));
    }

    time -= T(2.625 / 2.75);
    auto postFix = time;
    return 1 - (T(7.5625) * postFix * time + T(0.984375));
}

template <typename T>
T Back(T time) noexcept
{
    static_assert(std::is_floating_point<T>::value,
        "You can only use floating-point types");

    // Back easing
    constexpr auto s = T(1.70158);
    return time * time * ((s + 1) * time - s);
}

// NOTE: explicit instantiations
template f32 Quadratic<f32>(f32 time) noexcept;
template f32 Cubic(f32 time) noexcept;
template f32 Quartic(f32 time) noexcept;
template f32 Quintic(f32 time) noexcept;
template f32 Sine(f32 time) noexcept;
template f32 Exponential(f32 time) noexcept;
template f32 Circle(f32 time) noexcept;
template f32 Elastic(f32 time) noexcept;
template f32 Bounce(f32 time) noexcept;
template f32 Back(f32 time) noexcept;

} // namespace pomdog::detail::Easings
