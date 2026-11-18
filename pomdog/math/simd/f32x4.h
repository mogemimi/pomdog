// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/compilation_target_architecture.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#if defined(POMDOG_ARCHITECTURE_AMD64)
#include <xmmintrin.h>
#elif defined(POMDOG_ARCHITECTURE_ARM64)
#include <arm_neon.h>
#elif defined(__wasm_simd128__)
#include <wasm_simd128.h>
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

// NOTE: x86-64 always has SSE and AArch64 always has NEON, so those
// implementations need no runtime check. The WebAssembly SIMD implementation is
// used when the build passes -msimd128, as Pomdog's Emscripten build does.
// Other builds use F32x4Scalar.

namespace pomdog::math::simd {

/// F32x4Scalar holds four f32 lanes in plain memory and implements the F32x4
/// operations without SIMD instructions. It is available on every platform, so
/// code and tests can compare a SIMD result with it.
struct F32x4Scalar final {
    f32 lanes[4];
};

#if defined(POMDOG_ARCHITECTURE_AMD64)
/// F32x4 holds four f32 lanes in the SIMD register type of the target platform,
/// or is F32x4Scalar where Pomdog has no SIMD implementation.
using F32x4 = __m128;

/// HasSIMD is true when F32x4 is a SIMD register type and false when it is
/// F32x4Scalar, so that code can keep a separate version for scalars where
/// computing four lanes at once does not pay off.
inline constexpr bool HasSIMD = true;
#elif defined(POMDOG_ARCHITECTURE_ARM64)
using F32x4 = float32x4_t;
inline constexpr bool HasSIMD = true;
#elif defined(__wasm_simd128__)
using F32x4 = v128_t;
inline constexpr bool HasSIMD = true;
#else
using F32x4 = F32x4Scalar;
inline constexpr bool HasSIMD = false;
#endif

// NOTE: The operations below are overloaded for F32x4Scalar and for the SIMD
// type of F32x4, so both are usable on platforms that have SIMD. Only load() and
// splat() take the type as a template argument, as in load<F32x4>(values).

/// Returns a value of type V whose lanes are the four elements of source.
template <typename V>
[[nodiscard]] V
load(const f32 (&source)[4]) noexcept;

/// Returns a value of type V whose four lanes are all value.
template <typename V>
[[nodiscard]] V
splat(f32 value) noexcept;

template <>
[[nodiscard]] inline F32x4Scalar
load<F32x4Scalar>(const f32 (&source)[4]) noexcept
{
    return F32x4Scalar{{source[0], source[1], source[2], source[3]}};
}

template <>
[[nodiscard]] inline F32x4Scalar
splat<F32x4Scalar>(f32 value) noexcept
{
    return F32x4Scalar{{value, value, value, value}};
}

/// Writes the four lanes of v to destination.
inline void
store(f32 (&destination)[4], F32x4Scalar v) noexcept
{
    destination[0] = v.lanes[0];
    destination[1] = v.lanes[1];
    destination[2] = v.lanes[2];
    destination[3] = v.lanes[3];
}

/// Returns a + b for each lane.
[[nodiscard]] inline F32x4Scalar
add(F32x4Scalar a, F32x4Scalar b) noexcept
{
    return F32x4Scalar{{
        a.lanes[0] + b.lanes[0],
        a.lanes[1] + b.lanes[1],
        a.lanes[2] + b.lanes[2],
        a.lanes[3] + b.lanes[3],
    }};
}

/// Returns a - b for each lane.
[[nodiscard]] inline F32x4Scalar
subtract(F32x4Scalar a, F32x4Scalar b) noexcept
{
    return F32x4Scalar{{
        a.lanes[0] - b.lanes[0],
        a.lanes[1] - b.lanes[1],
        a.lanes[2] - b.lanes[2],
        a.lanes[3] - b.lanes[3],
    }};
}

/// Returns a * b for each lane.
[[nodiscard]] inline F32x4Scalar
multiply(F32x4Scalar a, F32x4Scalar b) noexcept
{
    return F32x4Scalar{{
        a.lanes[0] * b.lanes[0],
        a.lanes[1] * b.lanes[1],
        a.lanes[2] * b.lanes[2],
        a.lanes[3] * b.lanes[3],
    }};
}

/// Returns the smaller of a and b for each lane. For lanes that hold NaN, or
/// zeros of both signs, which lane the result takes depends on the platform.
[[nodiscard]] inline F32x4Scalar
min(F32x4Scalar a, F32x4Scalar b) noexcept
{
    return F32x4Scalar{{
        (b.lanes[0] < a.lanes[0]) ? b.lanes[0] : a.lanes[0],
        (b.lanes[1] < a.lanes[1]) ? b.lanes[1] : a.lanes[1],
        (b.lanes[2] < a.lanes[2]) ? b.lanes[2] : a.lanes[2],
        (b.lanes[3] < a.lanes[3]) ? b.lanes[3] : a.lanes[3],
    }};
}

/// Returns the larger of a and b for each lane. For lanes that hold NaN, or
/// zeros of both signs, which lane the result takes depends on the platform.
[[nodiscard]] inline F32x4Scalar
max(F32x4Scalar a, F32x4Scalar b) noexcept
{
    return F32x4Scalar{{
        (a.lanes[0] < b.lanes[0]) ? b.lanes[0] : a.lanes[0],
        (a.lanes[1] < b.lanes[1]) ? b.lanes[1] : a.lanes[1],
        (a.lanes[2] < b.lanes[2]) ? b.lanes[2] : a.lanes[2],
        (a.lanes[3] < b.lanes[3]) ? b.lanes[3] : a.lanes[3],
    }};
}

/// Returns a / b for each lane, computed by an exact division.
[[nodiscard]] inline F32x4Scalar
divide(F32x4Scalar a, F32x4Scalar b) noexcept
{
    return F32x4Scalar{{
        a.lanes[0] / b.lanes[0],
        a.lanes[1] / b.lanes[1],
        a.lanes[2] / b.lanes[2],
        a.lanes[3] / b.lanes[3],
    }};
}

/// Returns 1 / v for each lane, computed by an exact division.
[[nodiscard]] inline F32x4Scalar
reciprocal(F32x4Scalar v) noexcept
{
    return F32x4Scalar{{
        1.0f / v.lanes[0],
        1.0f / v.lanes[1],
        1.0f / v.lanes[2],
        1.0f / v.lanes[3],
    }};
}

/// Returns a - b * c for each lane. NEON uses fused multiply-subtract.
[[nodiscard]] inline F32x4Scalar
subtractProduct(F32x4Scalar a, F32x4Scalar b, F32x4Scalar c) noexcept
{
    return subtract(a, multiply(b, c));
}

/// Returns a + b * c for each lane. NEON uses fused multiply-add.
[[nodiscard]] inline F32x4Scalar
addProduct(F32x4Scalar a, F32x4Scalar b, F32x4Scalar c) noexcept
{
    return add(a, multiply(b, c));
}

/// Returns lane 0 of v.
[[nodiscard]] inline f32
firstLane(F32x4Scalar v) noexcept
{
    return v.lanes[0];
}

/// Returns a value whose lanes are lanes X, Y, Z, and W of v.
template <i32 X, i32 Y, i32 Z, i32 W>
[[nodiscard]] inline F32x4Scalar
swizzle(F32x4Scalar v) noexcept
{
    static_assert(X >= 0 && X < 4 && Y >= 0 && Y < 4 && Z >= 0 && Z < 4 && W >= 0 && W < 4);
    return F32x4Scalar{{v.lanes[X], v.lanes[Y], v.lanes[Z], v.lanes[W]}};
}

/// Returns a value whose lanes are lanes X and Y of a and lanes Z and W of b.
template <i32 X, i32 Y, i32 Z, i32 W>
[[nodiscard]] inline F32x4Scalar
shuffle(F32x4Scalar a, F32x4Scalar b) noexcept
{
    static_assert(X >= 0 && X < 4 && Y >= 0 && Y < 4 && Z >= 0 && Z < 4 && W >= 0 && W < 4);
    return F32x4Scalar{{a.lanes[X], a.lanes[Y], b.lanes[Z], b.lanes[W]}};
}

/// Writes c0, c1, c2, and c3 as the columns of rows, so that rows[i][j] is lane
/// i of column j.
inline void
storeTransposed(f32 (&rows)[4][4], F32x4Scalar c0, F32x4Scalar c1, F32x4Scalar c2, F32x4Scalar c3) noexcept
{
    rows[0][0] = c0.lanes[0];
    rows[0][1] = c1.lanes[0];
    rows[0][2] = c2.lanes[0];
    rows[0][3] = c3.lanes[0];
    rows[1][0] = c0.lanes[1];
    rows[1][1] = c1.lanes[1];
    rows[1][2] = c2.lanes[1];
    rows[1][3] = c3.lanes[1];
    rows[2][0] = c0.lanes[2];
    rows[2][1] = c1.lanes[2];
    rows[2][2] = c2.lanes[2];
    rows[2][3] = c3.lanes[2];
    rows[3][0] = c0.lanes[3];
    rows[3][1] = c1.lanes[3];
    rows[3][2] = c2.lanes[3];
    rows[3][3] = c3.lanes[3];
}

#if defined(POMDOG_ARCHITECTURE_AMD64)

template <>
[[nodiscard]] inline F32x4
load<F32x4>(const f32 (&source)[4]) noexcept
{
    return _mm_loadu_ps(source);
}

template <>
[[nodiscard]] inline F32x4
splat<F32x4>(f32 value) noexcept
{
    return _mm_set1_ps(value);
}

inline void
store(f32 (&destination)[4], F32x4 v) noexcept
{
    _mm_storeu_ps(destination, v);
}

[[nodiscard]] inline F32x4
add(F32x4 a, F32x4 b) noexcept
{
    return _mm_add_ps(a, b);
}

[[nodiscard]] inline F32x4
subtract(F32x4 a, F32x4 b) noexcept
{
    return _mm_sub_ps(a, b);
}

[[nodiscard]] inline F32x4
multiply(F32x4 a, F32x4 b) noexcept
{
    return _mm_mul_ps(a, b);
}

[[nodiscard]] inline F32x4
min(F32x4 a, F32x4 b) noexcept
{
    return _mm_min_ps(a, b);
}

[[nodiscard]] inline F32x4
max(F32x4 a, F32x4 b) noexcept
{
    return _mm_max_ps(a, b);
}

[[nodiscard]] inline F32x4
divide(F32x4 a, F32x4 b) noexcept
{
    return _mm_div_ps(a, b);
}

[[nodiscard]] inline F32x4
reciprocal(F32x4 v) noexcept
{
    return _mm_div_ps(_mm_set1_ps(1.0f), v);
}

[[nodiscard]] inline F32x4
subtractProduct(F32x4 a, F32x4 b, F32x4 c) noexcept
{
    return _mm_sub_ps(a, _mm_mul_ps(b, c));
}

[[nodiscard]] inline F32x4
addProduct(F32x4 a, F32x4 b, F32x4 c) noexcept
{
    return _mm_add_ps(a, _mm_mul_ps(b, c));
}

[[nodiscard]] inline f32
firstLane(F32x4 v) noexcept
{
    return _mm_cvtss_f32(v);
}

template <i32 X, i32 Y, i32 Z, i32 W>
[[nodiscard]] inline F32x4
swizzle(F32x4 v) noexcept
{
    static_assert(X >= 0 && X < 4 && Y >= 0 && Y < 4 && Z >= 0 && Z < 4 && W >= 0 && W < 4);
    return _mm_shuffle_ps(v, v, _MM_SHUFFLE(W, Z, Y, X));
}

template <i32 X, i32 Y, i32 Z, i32 W>
[[nodiscard]] inline F32x4
shuffle(F32x4 a, F32x4 b) noexcept
{
    static_assert(X >= 0 && X < 4 && Y >= 0 && Y < 4 && Z >= 0 && Z < 4 && W >= 0 && W < 4);
    return _mm_shuffle_ps(a, b, _MM_SHUFFLE(W, Z, Y, X));
}

inline void
storeTransposed(f32 (&rows)[4][4], F32x4 c0, F32x4 c1, F32x4 c2, F32x4 c3) noexcept
{
    const F32x4 t0 = _mm_unpacklo_ps(c0, c1);
    const F32x4 t1 = _mm_unpacklo_ps(c2, c3);
    const F32x4 t2 = _mm_unpackhi_ps(c0, c1);
    const F32x4 t3 = _mm_unpackhi_ps(c2, c3);
    _mm_storeu_ps(rows[0], _mm_movelh_ps(t0, t1));
    _mm_storeu_ps(rows[1], _mm_movehl_ps(t1, t0));
    _mm_storeu_ps(rows[2], _mm_movelh_ps(t2, t3));
    _mm_storeu_ps(rows[3], _mm_movehl_ps(t3, t2));
}

#elif defined(POMDOG_ARCHITECTURE_ARM64)

template <>
[[nodiscard]] inline F32x4
load<F32x4>(const f32 (&source)[4]) noexcept
{
    return vld1q_f32(source);
}

template <>
[[nodiscard]] inline F32x4
splat<F32x4>(f32 value) noexcept
{
    return vdupq_n_f32(value);
}

inline void
store(f32 (&destination)[4], F32x4 v) noexcept
{
    vst1q_f32(destination, v);
}

[[nodiscard]] inline F32x4
add(F32x4 a, F32x4 b) noexcept
{
    return vaddq_f32(a, b);
}

[[nodiscard]] inline F32x4
subtract(F32x4 a, F32x4 b) noexcept
{
    return vsubq_f32(a, b);
}

[[nodiscard]] inline F32x4
multiply(F32x4 a, F32x4 b) noexcept
{
    return vmulq_f32(a, b);
}

[[nodiscard]] inline F32x4
min(F32x4 a, F32x4 b) noexcept
{
    return vminq_f32(a, b);
}

[[nodiscard]] inline F32x4
max(F32x4 a, F32x4 b) noexcept
{
    return vmaxq_f32(a, b);
}

[[nodiscard]] inline F32x4
divide(F32x4 a, F32x4 b) noexcept
{
    return vdivq_f32(a, b);
}

[[nodiscard]] inline F32x4
reciprocal(F32x4 v) noexcept
{
    return vdivq_f32(vdupq_n_f32(1.0f), v);
}

// NOTE: AArch64 fused multiply-add rounds only the final result.
[[nodiscard]] inline F32x4
subtractProduct(F32x4 a, F32x4 b, F32x4 c) noexcept
{
    return vfmsq_f32(a, b, c);
}

[[nodiscard]] inline F32x4
addProduct(F32x4 a, F32x4 b, F32x4 c) noexcept
{
    return vfmaq_f32(a, b, c);
}

[[nodiscard]] inline f32
firstLane(F32x4 v) noexcept
{
    return vgetq_lane_f32(v, 0);
}

template <i32 X, i32 Y, i32 Z, i32 W>
[[nodiscard]] inline F32x4
swizzle(F32x4 v) noexcept
{
    static_assert(X >= 0 && X < 4 && Y >= 0 && Y < 4 && Z >= 0 && Z < 4 && W >= 0 && W < 4);

    // NOTE: NEON has no general four-lane permute intrinsic, so this uses a
    // byte table lookup (TBL). Clang turns these constant tables into cheaper
    // permutes such as REV64, EXT, and DUP. GCC keeps one TBL per swizzle, so
    // broadcasts use DUP directly.
    if constexpr (X == Y && X == Z && X == W) {
        return vdupq_laneq_f32(v, X);
    }
    else {
        static constexpr u8 indices[16] = {
            static_cast<u8>(X * 4),
            static_cast<u8>(X * 4 + 1),
            static_cast<u8>(X * 4 + 2),
            static_cast<u8>(X * 4 + 3),
            static_cast<u8>(Y * 4),
            static_cast<u8>(Y * 4 + 1),
            static_cast<u8>(Y * 4 + 2),
            static_cast<u8>(Y * 4 + 3),
            static_cast<u8>(Z * 4),
            static_cast<u8>(Z * 4 + 1),
            static_cast<u8>(Z * 4 + 2),
            static_cast<u8>(Z * 4 + 3),
            static_cast<u8>(W * 4),
            static_cast<u8>(W * 4 + 1),
            static_cast<u8>(W * 4 + 2),
            static_cast<u8>(W * 4 + 3),
        };
        return vreinterpretq_f32_u8(vqtbl1q_u8(vreinterpretq_u8_f32(v), vld1q_u8(indices)));
    }
}

template <i32 X, i32 Y, i32 Z, i32 W>
[[nodiscard]] inline F32x4
shuffle(F32x4 a, F32x4 b) noexcept
{
    static_assert(X >= 0 && X < 4 && Y >= 0 && Y < 4 && Z >= 0 && Z < 4 && W >= 0 && W < 4);

    // NOTE: A byte table lookup over both vectors (TBL with two registers),
    // where bytes 16 to 31 are those of b, as in swizzle().
    static constexpr u8 indices[16] = {
        static_cast<u8>(X * 4),
        static_cast<u8>(X * 4 + 1),
        static_cast<u8>(X * 4 + 2),
        static_cast<u8>(X * 4 + 3),
        static_cast<u8>(Y * 4),
        static_cast<u8>(Y * 4 + 1),
        static_cast<u8>(Y * 4 + 2),
        static_cast<u8>(Y * 4 + 3),
        static_cast<u8>(16 + Z * 4),
        static_cast<u8>(16 + Z * 4 + 1),
        static_cast<u8>(16 + Z * 4 + 2),
        static_cast<u8>(16 + Z * 4 + 3),
        static_cast<u8>(16 + W * 4),
        static_cast<u8>(16 + W * 4 + 1),
        static_cast<u8>(16 + W * 4 + 2),
        static_cast<u8>(16 + W * 4 + 3),
    };
    const uint8x16x2_t table = {{vreinterpretq_u8_f32(a), vreinterpretq_u8_f32(b)}};
    return vreinterpretq_f32_u8(vqtbl2q_u8(table, vld1q_u8(indices)));
}

inline void
storeTransposed(f32 (&rows)[4][4], F32x4 c0, F32x4 c1, F32x4 c2, F32x4 c3) noexcept
{
    const F32x4 t0 = vzip1q_f32(c0, c1);
    const F32x4 t1 = vzip1q_f32(c2, c3);
    const F32x4 t2 = vzip2q_f32(c0, c1);
    const F32x4 t3 = vzip2q_f32(c2, c3);
    vst1q_f32(rows[0], vcombine_f32(vget_low_f32(t0), vget_low_f32(t1)));
    vst1q_f32(rows[1], vcombine_f32(vget_high_f32(t0), vget_high_f32(t1)));
    vst1q_f32(rows[2], vcombine_f32(vget_low_f32(t2), vget_low_f32(t3)));
    vst1q_f32(rows[3], vcombine_f32(vget_high_f32(t2), vget_high_f32(t3)));
}

#elif defined(__wasm_simd128__)

template <>
[[nodiscard]] inline F32x4
load<F32x4>(const f32 (&source)[4]) noexcept
{
    return wasm_v128_load(source);
}

template <>
[[nodiscard]] inline F32x4
splat<F32x4>(f32 value) noexcept
{
    return wasm_f32x4_splat(value);
}

inline void
store(f32 (&destination)[4], F32x4 v) noexcept
{
    wasm_v128_store(destination, v);
}

[[nodiscard]] inline F32x4
add(F32x4 a, F32x4 b) noexcept
{
    return wasm_f32x4_add(a, b);
}

[[nodiscard]] inline F32x4
subtract(F32x4 a, F32x4 b) noexcept
{
    return wasm_f32x4_sub(a, b);
}

[[nodiscard]] inline F32x4
multiply(F32x4 a, F32x4 b) noexcept
{
    return wasm_f32x4_mul(a, b);
}

// NOTE: pmin and pmax are the comparisons that x86 MINPS and MAXPS make, so
// engines compile them to one instruction there, while min and max add
// instructions for the NaN and signed zero rules of WebAssembly.
[[nodiscard]] inline F32x4
min(F32x4 a, F32x4 b) noexcept
{
    return wasm_f32x4_pmin(a, b);
}

[[nodiscard]] inline F32x4
max(F32x4 a, F32x4 b) noexcept
{
    return wasm_f32x4_pmax(a, b);
}

[[nodiscard]] inline F32x4
divide(F32x4 a, F32x4 b) noexcept
{
    return wasm_f32x4_div(a, b);
}

[[nodiscard]] inline F32x4
reciprocal(F32x4 v) noexcept
{
    return wasm_f32x4_div(wasm_f32x4_splat(1.0f), v);
}

[[nodiscard]] inline F32x4
subtractProduct(F32x4 a, F32x4 b, F32x4 c) noexcept
{
    return wasm_f32x4_sub(a, wasm_f32x4_mul(b, c));
}

[[nodiscard]] inline F32x4
addProduct(F32x4 a, F32x4 b, F32x4 c) noexcept
{
    return wasm_f32x4_add(a, wasm_f32x4_mul(b, c));
}

[[nodiscard]] inline f32
firstLane(F32x4 v) noexcept
{
    return wasm_f32x4_extract_lane(v, 0);
}

template <i32 X, i32 Y, i32 Z, i32 W>
[[nodiscard]] inline F32x4
swizzle(F32x4 v) noexcept
{
    static_assert(X >= 0 && X < 4 && Y >= 0 && Y < 4 && Z >= 0 && Z < 4 && W >= 0 && W < 4);
    return wasm_i32x4_shuffle(v, v, X, Y, Z, W);
}

template <i32 X, i32 Y, i32 Z, i32 W>
[[nodiscard]] inline F32x4
shuffle(F32x4 a, F32x4 b) noexcept
{
    static_assert(X >= 0 && X < 4 && Y >= 0 && Y < 4 && Z >= 0 && Z < 4 && W >= 0 && W < 4);
    return wasm_i32x4_shuffle(a, b, X, Y, Z + 4, W + 4);
}

inline void
storeTransposed(f32 (&rows)[4][4], F32x4 c0, F32x4 c1, F32x4 c2, F32x4 c3) noexcept
{
    const F32x4 t0 = wasm_i32x4_shuffle(c0, c1, 0, 4, 1, 5);
    const F32x4 t1 = wasm_i32x4_shuffle(c2, c3, 0, 4, 1, 5);
    const F32x4 t2 = wasm_i32x4_shuffle(c0, c1, 2, 6, 3, 7);
    const F32x4 t3 = wasm_i32x4_shuffle(c2, c3, 2, 6, 3, 7);
    wasm_v128_store(rows[0], wasm_i32x4_shuffle(t0, t1, 0, 1, 4, 5));
    wasm_v128_store(rows[1], wasm_i32x4_shuffle(t0, t1, 2, 3, 6, 7));
    wasm_v128_store(rows[2], wasm_i32x4_shuffle(t2, t3, 0, 1, 4, 5));
    wasm_v128_store(rows[3], wasm_i32x4_shuffle(t2, t3, 2, 3, 6, 7));
}

#endif

} // namespace pomdog::math::simd
