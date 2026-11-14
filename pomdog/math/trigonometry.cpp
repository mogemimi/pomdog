// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/trigonometry.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <limits>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::math::detail {
namespace {

// NOTE: The kernels approximate sin(pi * r) and cos(pi * r) for r in
// [-0.25, 0.25] with minimax polynomials in s = r * r that minimize the
// relative error. Each coefficient was rounded to the floating-point type in
// turn, and the remaining ones were refitted after each rounding, which keeps
// the polynomial error below 0.07 ulp.
//
// The sine kernels split pi into a head with half the significant bits of
// the type and a tail. With rh holding the upper half of the bits of r,
// rh * head is exact, and the result is rounded only once in the final
// addition. Multiplying r by pi rounded to the type would instead cost up to
// about 1 ulp more.

/// Returns sin(pi * r) for r in [-0.25, 0.25], where s is r * r.
[[nodiscard]] f32
sinpiKernel(f32 r, f32 s) noexcept
{
    // NOTE: 3.1416015625f has 12 significant bits, and rh keeps 12 bits of r.
    const f32 rh = std::bit_cast<f32>(std::bit_cast<u32>(r) & 0xFFFFF000u);
    const f32 rl = r - rh;

    f32 p = -0.589277685f;
    p = p * s + 2.54979467f;
    p = p * s + -5.16770887f;
    return rh * 3.1416015625f + ((rl * 3.1416015625f + r * -8.90890988e-06f) + (r * s) * p);
}

/// Returns sin(pi * r) for r in [-0.25, 0.25], where s is r * r.
[[nodiscard]] f64
sinpiKernel(f64 r, f64 s) noexcept
{
    // NOTE: 3.1415926814079285 has 26 significant bits, and rh keeps 26 bits of r.
    const f64 rh = std::bit_cast<f64>(std::bit_cast<u64>(r) & 0xFFFFFFFFF8000000u);
    const f64 rl = r - rh;

    f64 p = 0.00046164817513834409;
    p = p * s + -0.0073700450279192749;
    p = p * s + 0.082145870985258551;
    p = p * s + -0.59926452900553695;
    p = p * s + 2.5501640398745122;
    p = p * s + -5.1677127800499623;
    return rh * 3.1415926814079285 + ((rl * 3.1415926814079285 + r * -2.7818135228334233e-08) + (r * s) * p);
}

/// Returns cos(pi * r) for r in [-0.25, 0.25], where s is r * r.
[[nodiscard]] f32
cospiKernel(f32 s) noexcept
{
    f32 p = 0.228724524f;
    p = p * s + -1.33473504f;
    p = p * s + 4.05869627f;
    p = p * s + -4.93480206f;
    return 1.0f + s * p;
}

/// Returns cos(pi * r) for r in [-0.25, 0.25], where s is r * r.
[[nodiscard]] f64
cospiKernel(f64 s) noexcept
{
    f64 p = -0.00010272515970435005;
    p = p * s + 0.0019292866167756181;
    p = p * s + -0.025806870858877243;
    p = p * s + 0.23533062959681794;
    p = p * s + -1.3352627688401233;
    p = p * s + 4.058712126416645;
    p = p * s + -4.934802200544679;
    return 1.0 + s * p;
}

/// Returns value negated when x has its sign bit set, including x == -0.
[[nodiscard]] f32
applySignOf(f32 value, f32 x) noexcept
{
    constexpr u32 signMask = 0x80000000u;
    return std::bit_cast<f32>(std::bit_cast<u32>(value) ^ (std::bit_cast<u32>(x) & signMask));
}

/// Returns value negated when x has its sign bit set, including x == -0.
[[nodiscard]] f64
applySignOf(f64 value, f64 x) noexcept
{
    constexpr u64 signMask = 0x8000000000000000u;
    return std::bit_cast<f64>(std::bit_cast<u64>(value) ^ (std::bit_cast<u64>(x) & signMask));
}

/// The bound below which reduceHalfTurns rounds |x| to a multiple of 0.5 without
/// calling std::fmod. It is 2^21 for f32 and 2^50 for f64.
template <typename T>
constexpr T fastReductionLimit = static_cast<T>(u64{1} << (std::numeric_limits<T>::digits - 3));

/// Shifts the input into a range with spacing 0.5. Adding and then subtracting
/// it rounds |x| below fastReductionLimit to the nearest multiple of 0.5. It is
/// 1.5 * 2^22 for f32 and 1.5 * 2^51 for f64.
template <typename T>
constexpr T roundingBias = static_cast<T>(u64{3} << (std::numeric_limits<T>::digits - 3));

/// Splits |x| into r in [-0.25, 0.25] and a quadrant q in [0, 3] so that
/// |x| = 2k + q / 2 + r for some integer k. Returns NaN for r when x is
/// infinite or NaN.
template <typename T>
[[nodiscard]] auto reduceHalfTurns(T x) noexcept -> std::tuple<T, i32>
{
    static_assert(std::is_floating_point_v<T>);

    T a = std::abs(x);

    // NOTE: The comparison is also false for NaN.
    if (!(a < fastReductionLimit<T>)) [[unlikely]] {
        if (!std::isfinite(a)) {
            return {std::numeric_limits<T>::quiet_NaN(), 0};
        }
        // NOTE: std::fmod is exact, so a keeps every bit of the fraction of |x|.
        a = std::fmod(a, T{2});
    }

    // NOTE: Adding the bias rounds a to the nearest multiple of 0.5. This
    // assumes round-to-nearest-even and no reassociation of the additions.
    const T biased = a + roundingBias<T>;
    const T nearestHalf = biased - roundingBias<T>;

    // NOTE: a and nearestHalf are both multiples of the spacing of floats
    // around a, so r is exact. This makes multiples of 0.5 give exact results.
    const T r = a - nearestHalf;

    // NOTE: The bias has two zero low bits. Each step of 0.5 increments
    // its bit pattern by one, so the low bits give the quadrant modulo 4.
    using Bits = std::conditional_t<std::is_same_v<T, f32>, u32, u64>;
    const auto quadrant = static_cast<i32>(std::bit_cast<Bits>(biased) & 3);
    return {r, quadrant};
}

/// Holds the factors that turn the kernel results into sin(pi * x) and
/// cos(pi * x) in one quadrant.
template <typename T>
struct QuadrantFactors final {
    T cosine;
    T sine;
};

/// Stores the exact cosine and sine of the four quadrant angles.
template <typename T>
constexpr std::array<QuadrantFactors<T>, 4> quadrantFactors = {{
    {T{1}, T{0}},
    {T{0}, T{1}},
    {T{-1}, T{0}},
    {T{0}, T{-1}},
}};

// NOTE: Each rotation multiplies one kernel result by zero and the other by
// +1 or -1. Positive zero in the table preserves +0 at quadrant boundaries.

template <typename T>
[[nodiscard]] auto sinpi(T x) noexcept -> T
{
    const auto [r, quadrant] = reduceHalfTurns(x);
    const T s = r * r;
    const T sinR = sinpiKernel(r, s);
    const T cosR = cospiKernel(s);
    const auto& f = quadrantFactors<T>[static_cast<std::size_t>(quadrant)];

    // NOTE: sin is odd, so this also gives sinpi(-0) == -0.
    return applySignOf(sinR * f.cosine + cosR * f.sine, x);
}

template <typename T>
[[nodiscard]] auto cospi(T x) noexcept -> T
{
    // NOTE: cos is even, so the sign of x does not matter.
    const auto [r, quadrant] = reduceHalfTurns(x);
    const T s = r * r;
    const T sinR = sinpiKernel(r, s);
    const T cosR = cospiKernel(s);
    const auto& f = quadrantFactors<T>[static_cast<std::size_t>(quadrant)];
    return cosR * f.cosine - sinR * f.sine;
}

template <typename T>
[[nodiscard]] auto sincospi(T x) noexcept -> std::tuple<T, T>
{
    const auto [r, quadrant] = reduceHalfTurns(x);
    const T s = r * r;
    const T sinR = sinpiKernel(r, s);
    const T cosR = cospiKernel(s);
    const auto& f = quadrantFactors<T>[static_cast<std::size_t>(quadrant)];
    return {
        applySignOf(sinR * f.cosine + cosR * f.sine, x),
        cosR * f.cosine - sinR * f.sine,
    };
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
