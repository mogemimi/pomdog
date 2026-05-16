// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/easing.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/math_constants.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::math {

[[nodiscard]] f32
easeLinear(f32 x) noexcept
{
    return x;
}

[[nodiscard]] f64
easeLinear(f64 x) noexcept
{
    return x;
}

// MARK: Quadratic (Quad)

namespace {

template <typename T>
[[nodiscard]] T
easeInQuadImpl(T x) noexcept
{
    return x * x;
}

template <typename T>
[[nodiscard]] T
easeOutQuadImpl(T x) noexcept
{
    const auto y = T(1) - x;
    return T(1) - y * y;
}

template <typename T>
[[nodiscard]] T
easeInOutQuadImpl(T x) noexcept
{
    if (x < T(0.5)) {
        return T(2) * x * x;
    }
    const auto y = T(-2) * x + T(2);
    return T(1) - y * y * T(0.5);
}

} // namespace

[[nodiscard]] f32
easeInQuad(f32 x) noexcept
{
    return easeInQuadImpl(x);
}

[[nodiscard]] f64
easeInQuad(f64 x) noexcept
{
    return easeInQuadImpl(x);
}

[[nodiscard]] f32
easeOutQuad(f32 x) noexcept
{
    return easeOutQuadImpl(x);
}

[[nodiscard]] f64
easeOutQuad(f64 x) noexcept
{
    return easeOutQuadImpl(x);
}

[[nodiscard]] f32
easeInOutQuad(f32 x) noexcept
{
    return easeInOutQuadImpl(x);
}

[[nodiscard]] f64
easeInOutQuad(f64 x) noexcept
{
    return easeInOutQuadImpl(x);
}

// MARK: Cubic (Cubic)

namespace {

template <typename T>
[[nodiscard]] T
easeInCubicImpl(T x) noexcept
{
    return x * x * x;
}

template <typename T>
[[nodiscard]] T
easeOutCubicImpl(T x) noexcept
{
    const auto y = T(1) - x;
    return T(1) - y * y * y;
}

template <typename T>
[[nodiscard]] T
easeInOutCubicImpl(T x) noexcept
{
    if (x < T(0.5)) {
        return T(4) * x * x * x;
    }
    const auto y = T(-2) * x + T(2);
    return T(1) - y * y * y * T(0.5);
}

} // namespace

[[nodiscard]] f32
easeInCubic(f32 x) noexcept
{
    return easeInCubicImpl(x);
}

[[nodiscard]] f64
easeInCubic(f64 x) noexcept
{
    return easeInCubicImpl(x);
}

[[nodiscard]] f32
easeOutCubic(f32 x) noexcept
{
    return easeOutCubicImpl(x);
}

[[nodiscard]] f64
easeOutCubic(f64 x) noexcept
{
    return easeOutCubicImpl(x);
}

[[nodiscard]] f32
easeInOutCubic(f32 x) noexcept
{
    return easeInOutCubicImpl(x);
}

[[nodiscard]] f64
easeInOutCubic(f64 x) noexcept
{
    return easeInOutCubicImpl(x);
}

// MARK: Quartic (Quart)

namespace {

template <typename T>
[[nodiscard]] T
easeInQuartImpl(T x) noexcept
{
    return x * x * x * x;
}

template <typename T>
[[nodiscard]] T
easeOutQuartImpl(T x) noexcept
{
    const auto y = T(1) - x;
    return T(1) - y * y * y * y;
}

template <typename T>
[[nodiscard]] T
easeInOutQuartImpl(T x) noexcept
{
    if (x < T(0.5)) {
        return T(8) * x * x * x * x;
    }
    const auto y = T(-2) * x + T(2);
    return T(1) - y * y * y * y * T(0.5);
}

} // namespace

[[nodiscard]] f32
easeInQuart(f32 x) noexcept
{
    return easeInQuartImpl(x);
}

[[nodiscard]] f64
easeInQuart(f64 x) noexcept
{
    return easeInQuartImpl(x);
}

[[nodiscard]] f32
easeOutQuart(f32 x) noexcept
{
    return easeOutQuartImpl(x);
}

[[nodiscard]] f64
easeOutQuart(f64 x) noexcept
{
    return easeOutQuartImpl(x);
}

[[nodiscard]] f32
easeInOutQuart(f32 x) noexcept
{
    return easeInOutQuartImpl(x);
}

[[nodiscard]] f64
easeInOutQuart(f64 x) noexcept
{
    return easeInOutQuartImpl(x);
}

// MARK: Quintic (Quint)

namespace {

template <typename T>
[[nodiscard]] T
easeInQuintImpl(T x) noexcept
{
    return x * x * x * x * x;
}

template <typename T>
[[nodiscard]] T
easeOutQuintImpl(T x) noexcept
{
    const auto y = T(1) - x;
    return T(1) - y * y * y * y * y;
}

template <typename T>
[[nodiscard]] T
easeInOutQuintImpl(T x) noexcept
{
    if (x < T(0.5)) {
        return T(16) * x * x * x * x * x;
    }
    const auto y = T(-2) * x + T(2);
    return T(1) - y * y * y * y * y * T(0.5);
}

} // namespace

[[nodiscard]] f32
easeInQuint(f32 x) noexcept
{
    return easeInQuintImpl(x);
}

[[nodiscard]] f64
easeInQuint(f64 x) noexcept
{
    return easeInQuintImpl(x);
}

[[nodiscard]] f32
easeOutQuint(f32 x) noexcept
{
    return easeOutQuintImpl(x);
}

[[nodiscard]] f64
easeOutQuint(f64 x) noexcept
{
    return easeOutQuintImpl(x);
}

[[nodiscard]] f32
easeInOutQuint(f32 x) noexcept
{
    return easeInOutQuintImpl(x);
}

[[nodiscard]] f64
easeInOutQuint(f64 x) noexcept
{
    return easeInOutQuintImpl(x);
}

// MARK: Sinusoidal (Sine)

namespace {

template <typename T>
[[nodiscard]] T
easeInSineImpl(T x) noexcept
{
    return T(1) - std::cos(x * math::PiOver2<T>);
}

template <typename T>
[[nodiscard]] T
easeOutSineImpl(T x) noexcept
{
    return std::sin(x * math::PiOver2<T>);
}

template <typename T>
[[nodiscard]] T
easeInOutSineImpl(T x) noexcept
{
    return -(std::cos(math::Pi<T> * x) - T(1)) * T(0.5);
}

} // namespace

[[nodiscard]] f32
easeInSine(f32 x) noexcept
{
    return easeInSineImpl(x);
}

[[nodiscard]] f64
easeInSine(f64 x) noexcept
{
    return easeInSineImpl(x);
}

[[nodiscard]] f32
easeOutSine(f32 x) noexcept
{
    return easeOutSineImpl(x);
}

[[nodiscard]] f64
easeOutSine(f64 x) noexcept
{
    return easeOutSineImpl(x);
}

[[nodiscard]] f32
easeInOutSine(f32 x) noexcept
{
    return easeInOutSineImpl(x);
}

[[nodiscard]] f64
easeInOutSine(f64 x) noexcept
{
    return easeInOutSineImpl(x);
}

// MARK: Exponential (Expo)

namespace {

template <typename T>
[[nodiscard]] T
easeInExpoImpl(T x) noexcept
{
    if (x == T(0)) {
        return T(0);
    }
    return std::pow(T(2), T(10) * x - T(10));
}

template <typename T>
[[nodiscard]] T
easeOutExpoImpl(T x) noexcept
{
    if (x == T(1)) {
        return T(1);
    }
    return T(1) - std::pow(T(2), T(-10) * x);
}

template <typename T>
[[nodiscard]] T
easeInOutExpoImpl(T x) noexcept
{
    if (x == T(0)) {
        return T(0);
    }
    if (x == T(1)) {
        return T(1);
    }
    if (x < T(0.5)) {
        return std::pow(T(2), T(20) * x - T(10)) * T(0.5);
    }
    return (T(2) - std::pow(T(2), T(-20) * x + T(10))) * T(0.5);
}

} // namespace

[[nodiscard]] f32
easeInExpo(f32 x) noexcept
{
    return easeInExpoImpl(x);
}

[[nodiscard]] f64
easeInExpo(f64 x) noexcept
{
    return easeInExpoImpl(x);
}

[[nodiscard]] f32
easeOutExpo(f32 x) noexcept
{
    return easeOutExpoImpl(x);
}

[[nodiscard]] f64
easeOutExpo(f64 x) noexcept
{
    return easeOutExpoImpl(x);
}

[[nodiscard]] f32
easeInOutExpo(f32 x) noexcept
{
    return easeInOutExpoImpl(x);
}

[[nodiscard]] f64
easeInOutExpo(f64 x) noexcept
{
    return easeInOutExpoImpl(x);
}

// MARK: Circular (Circ)

namespace {

template <typename T>
[[nodiscard]] T
easeInCircImpl(T x) noexcept
{
    return T(1) - std::sqrt(T(1) - x * x);
}

template <typename T>
[[nodiscard]] T
easeOutCircImpl(T x) noexcept
{
    const auto y = x - T(1);
    return std::sqrt(T(1) - y * y);
}

template <typename T>
[[nodiscard]] T
easeInOutCircImpl(T x) noexcept
{
    if (x < T(0.5)) {
        const auto y = T(2) * x;
        return (T(1) - std::sqrt(T(1) - y * y)) * T(0.5);
    }
    const auto y = T(-2) * x + T(2);
    return (std::sqrt(T(1) - y * y) + T(1)) * T(0.5);
}

} // namespace

[[nodiscard]] f32
easeInCirc(f32 x) noexcept
{
    return easeInCircImpl(x);
}

[[nodiscard]] f64
easeInCirc(f64 x) noexcept
{
    return easeInCircImpl(x);
}

[[nodiscard]] f32
easeOutCirc(f32 x) noexcept
{
    return easeOutCircImpl(x);
}

[[nodiscard]] f64
easeOutCirc(f64 x) noexcept
{
    return easeOutCircImpl(x);
}

[[nodiscard]] f32
easeInOutCirc(f32 x) noexcept
{
    return easeInOutCircImpl(x);
}

[[nodiscard]] f64
easeInOutCirc(f64 x) noexcept
{
    return easeInOutCircImpl(x);
}

// MARK: Elastic

namespace {

template <typename T>
[[nodiscard]] T
easeInElasticImpl(T x) noexcept
{
    if (x == T(0)) {
        return T(0);
    }
    if (x == T(1)) {
        return T(1);
    }
    constexpr auto c4 = math::TwoPi<T> / T(3);
    return -std::pow(T(2), T(10) * x - T(10)) * std::sin((x * T(10) - T(10.75)) * c4);
}

template <typename T>
[[nodiscard]] T
easeOutElasticImpl(T x) noexcept
{
    if (x == T(0)) {
        return T(0);
    }
    if (x == T(1)) {
        return T(1);
    }
    constexpr auto c4 = math::TwoPi<T> / T(3);
    return std::pow(T(2), T(-10) * x) * std::sin((x * T(10) - T(0.75)) * c4) + T(1);
}

template <typename T>
[[nodiscard]] T
easeInOutElasticImpl(T x) noexcept
{
    if (x == T(0)) {
        return T(0);
    }
    if (x == T(1)) {
        return T(1);
    }
    constexpr auto c5 = math::TwoPi<T> / T(4.5);
    if (x < T(0.5)) {
        return -(std::pow(T(2), T(20) * x - T(10)) * std::sin((T(20) * x - T(11.125)) * c5)) * T(0.5);
    }
    return (std::pow(T(2), T(-20) * x + T(10)) * std::sin((T(20) * x - T(11.125)) * c5)) * T(0.5) + T(1);
}

} // namespace

[[nodiscard]] f32
easeInElastic(f32 x) noexcept
{
    return easeInElasticImpl(x);
}

[[nodiscard]] f64
easeInElastic(f64 x) noexcept
{
    return easeInElasticImpl(x);
}

[[nodiscard]] f32
easeOutElastic(f32 x) noexcept
{
    return easeOutElasticImpl(x);
}

[[nodiscard]] f64
easeOutElastic(f64 x) noexcept
{
    return easeOutElasticImpl(x);
}

[[nodiscard]] f32
easeInOutElastic(f32 x) noexcept
{
    return easeInOutElasticImpl(x);
}

[[nodiscard]] f64
easeInOutElastic(f64 x) noexcept
{
    return easeInOutElasticImpl(x);
}

// MARK: Bounce

namespace {

template <typename T>
[[nodiscard]] T
easeOutBounceImpl(T x) noexcept
{
    constexpr T n1 = T(7.5625);
    constexpr T d1 = T(2.75);

    if (x < T(1) / d1) {
        return n1 * x * x;
    }
    else if (x < T(2) / d1) {
        x -= T(1.5) / d1;
        return n1 * x * x + T(0.75);
    }
    else if (x < T(2.5) / d1) {
        x -= T(2.25) / d1;
        return n1 * x * x + T(0.9375);
    }
    else {
        x -= T(2.625) / d1;
        return n1 * x * x + T(0.984375);
    }
}

template <typename T>
[[nodiscard]] T
easeInBounceImpl(T x) noexcept
{
    return T(1) - easeOutBounceImpl(T(1) - x);
}

template <typename T>
[[nodiscard]] T
easeInOutBounceImpl(T x) noexcept
{
    if (x < T(0.5)) {
        return (T(1) - easeOutBounceImpl(T(1) - T(2) * x)) * T(0.5);
    }
    return (T(1) + easeOutBounceImpl(T(2) * x - T(1))) * T(0.5);
}

} // namespace

[[nodiscard]] f32
easeOutBounce(f32 x) noexcept
{
    return easeOutBounceImpl(x);
}

[[nodiscard]] f64
easeOutBounce(f64 x) noexcept
{
    return easeOutBounceImpl(x);
}

[[nodiscard]] f32
easeInBounce(f32 x) noexcept
{
    return easeInBounceImpl(x);
}

[[nodiscard]] f64
easeInBounce(f64 x) noexcept
{
    return easeInBounceImpl(x);
}

[[nodiscard]] f32
easeInOutBounce(f32 x) noexcept
{
    return easeInOutBounceImpl(x);
}

[[nodiscard]] f64
easeInOutBounce(f64 x) noexcept
{
    return easeInOutBounceImpl(x);
}

// MARK: Back

namespace {

template <typename T>
[[nodiscard]] T
easeInBackImpl(T x) noexcept
{
    constexpr auto c1 = T(1.70158);
    constexpr auto c3 = c1 + T(1);
    return c3 * x * x * x - c1 * x * x;
}

template <typename T>
[[nodiscard]] T
easeOutBackImpl(T x) noexcept
{
    constexpr auto c1 = T(1.70158);
    constexpr auto c3 = c1 + T(1);
    const auto y = x - T(1);
    return T(1) + c3 * y * y * y + c1 * y * y;
}

template <typename T>
[[nodiscard]] T
easeInOutBackImpl(T x) noexcept
{
    constexpr auto c1 = T(1.70158);
    constexpr auto c2 = c1 * T(1.525);
    if (x < T(0.5)) {
        const auto y = T(2) * x;
        return (y * y * ((c2 + T(1)) * y - c2)) * T(0.5);
    }
    const auto y = T(2) * x - T(2);
    return (y * y * ((c2 + T(1)) * y + c2) + T(2)) * T(0.5);
}

} // namespace

[[nodiscard]] f32
easeInBack(f32 x) noexcept
{
    return easeInBackImpl(x);
}

[[nodiscard]] f64
easeInBack(f64 x) noexcept
{
    return easeInBackImpl(x);
}

[[nodiscard]] f32
easeOutBack(f32 x) noexcept
{
    return easeOutBackImpl(x);
}

[[nodiscard]] f64
easeOutBack(f64 x) noexcept
{
    return easeOutBackImpl(x);
}

[[nodiscard]] f32
easeInOutBack(f32 x) noexcept
{
    return easeInOutBackImpl(x);
}

[[nodiscard]] f64
easeInOutBack(f64 x) noexcept
{
    return easeInOutBackImpl(x);
}

} // namespace pomdog::math
