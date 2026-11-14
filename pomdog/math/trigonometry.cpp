// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/trigonometry.h"
#include "pomdog/math/math_constants.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <limits>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::math::detail {
namespace {

// TODO: Replace std::sin and std::cos below with polynomials over [-pi/4, pi/4].

/// Splits |x| into r in [-0.25, 0.25] and a quadrant q in [0, 3] so that
/// |x| = 2k + q / 2 + r for some integer k. x must be finite.
template <typename T>
[[nodiscard]] auto reduceHalfTurns(T x) noexcept -> std::tuple<T, i32>
{
    static_assert(std::is_floating_point_v<T>);

    // NOTE: std::fmod is exact, so y keeps every bit of the fraction of |x|,
    // even where pi * x would have lost it.
    const T y = std::fmod(std::abs(x), T{2});
    const T n = std::round(y * T{2});

    // NOTE: y and n / 2 are both multiples of the spacing of floats around y,
    // so r is exact. This is what makes multiples of 0.5 give exact results.
    const T r = y - n * T{0.5};
    return {r, static_cast<i32>(n) & 3};
}

template <typename T>
[[nodiscard]] auto sinpi(T x) noexcept -> T
{
    if (!std::isfinite(x)) {
        return std::numeric_limits<T>::quiet_NaN();
    }

    const auto [r, quadrant] = reduceHalfTurns(x);
    const T a = r * Pi<T>;

    T result = {};
    switch (quadrant) {
    case 0:
        result = std::sin(a);
        break;
    case 1:
        result = std::cos(a);
        break;
    case 2:
        // NOTE: Subtract from zero so that sinpi(1) returns +0 instead of -0.
        result = T{0} - std::sin(a);
        break;
    default:
        result = -std::cos(a);
        break;
    }

    // NOTE: sin is odd, so this also gives sinpi(-0) == -0.
    if (std::signbit(x)) {
        result = -result;
    }
    return result;
}

template <typename T>
[[nodiscard]] auto cospi(T x) noexcept -> T
{
    if (!std::isfinite(x)) {
        return std::numeric_limits<T>::quiet_NaN();
    }

    // NOTE: cos is even, so the sign of x does not matter.
    const auto [r, quadrant] = reduceHalfTurns(x);
    const T a = r * Pi<T>;

    switch (quadrant) {
    case 0:
        return std::cos(a);
    case 1:
        // NOTE: Subtract from zero so that cospi(0.5) returns +0 instead of -0.
        return T{0} - std::sin(a);
    case 2:
        return -std::cos(a);
    default:
        return std::sin(a);
    }
}

template <typename T>
[[nodiscard]] auto sincospi(T x) noexcept -> std::tuple<T, T>
{
    if (!std::isfinite(x)) {
        return {std::numeric_limits<T>::quiet_NaN(), std::numeric_limits<T>::quiet_NaN()};
    }

    const auto [r, quadrant] = reduceHalfTurns(x);
    const T a = r * Pi<T>;

    const T s = std::sin(a);
    const T c = std::cos(a);

    // NOTE: The cases match sinpi and cospi, including the signs of zero.
    T sinResult = {};
    T cosResult = {};
    switch (quadrant) {
    case 0:
        sinResult = s;
        cosResult = c;
        break;
    case 1:
        sinResult = c;
        cosResult = T{0} - s;
        break;
    case 2:
        sinResult = T{0} - s;
        cosResult = -c;
        break;
    default:
        sinResult = -c;
        cosResult = s;
        break;
    }

    if (std::signbit(x)) {
        sinResult = -sinResult;
    }
    return {sinResult, cosResult};
}

} // namespace
} // namespace pomdog::math::detail

namespace pomdog::math {

[[nodiscard]] f32
sinpi(f32 x) noexcept
{
    return detail::sinpi(x);
}

[[nodiscard]] f64
sinpi(f64 x) noexcept
{
    return detail::sinpi(x);
}

[[nodiscard]] f32
cospi(f32 x) noexcept
{
    return detail::cospi(x);
}

[[nodiscard]] f64
cospi(f64 x) noexcept
{
    return detail::cospi(x);
}

[[nodiscard]] std::tuple<f32, f32>
sincospi(f32 x) noexcept
{
    return detail::sincospi(x);
}

[[nodiscard]] std::tuple<f64, f64>
sincospi(f64 x) noexcept
{
    return detail::sincospi(x);
}

} // namespace pomdog::math
