// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
#include <type_traits>
#if defined(_MSC_VER)
#include <intrin.h>
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::random {

/// A cross-platform uniform integer distribution that produces identical
/// results on all platforms when given the same seed and generator state.
/// Unlike std::uniform_int_distribution, whose output varies by STL
/// implementation, this class guarantees deterministic results.
template <typename IntType = int>
class UniformIntDistribution final {
    static_assert(std::is_integral_v<IntType>, "IntType must be an integral type");
    static_assert(sizeof(IntType) <= 8, "IntType must be at most 64 bits");

public:
    using result_type = IntType;

private:
    IntType a_;
    IntType b_;

public:
    /// Creates a distribution over [0, std::numeric_limits<IntType>::max()].
    UniformIntDistribution() noexcept
        : a_(0)
        , b_(std::numeric_limits<IntType>::max())
    {
    }

    /// Creates a distribution over [a, b].
    UniformIntDistribution(IntType a, IntType b) noexcept
        : a_(a)
        , b_(b)
    {
        POMDOG_ASSERT(a <= b);
    }

    /// Generates a uniformly distributed random integer in [a, b].
    template <typename Generator>
    [[nodiscard]] result_type
    operator()(Generator& g) noexcept
    {
        static_assert(
            std::is_same_v<typename Generator::result_type, u64>,
            "Generator::result_type must be u64");
        static_assert(
            Generator::min() == 0,
            "Generator must produce values in [0, UINT64_MAX]");
        static_assert(
            Generator::max() == std::numeric_limits<u64>::max(),
            "Generator must produce values in [0, UINT64_MAX]");

        using UnsignedType = std::make_unsigned_t<IntType>;

        // NOTE: Compute the range [a, b] as an unsigned distance.
        // The outer cast to UnsignedType is needed for narrow types (e.g.
        // uint8_t, uint16_t) where integer promotion turns the unsigned
        // subtraction into a signed int subtraction, yielding a negative
        // result that would become a huge value when widened to u64.
        const u64 range = static_cast<u64>(static_cast<UnsignedType>(
            static_cast<UnsignedType>(b_) - static_cast<UnsignedType>(a_)));

        if (range == 0) {
            // NOTE: a == b; only one possible value.
            return a_;
        }

        if constexpr (sizeof(IntType) <= 4) {
            // NOTE: 32-bit fast path. For types up to 32 bits, the range
            // always fits in u32, so we can use a single 32×32→64 native
            // multiply instead of an expensive 128-bit multiplication.
            // NOTE: `g() >> 32` extracts the upper 32 bits of the generator's
            // u64 output as a uniform u32 value. This relies on
            // `Generator::min() == 0 && Generator::max() == UINT64_MAX`.
            // If this assumption is relaxed, update accordingly.
            const u32 range32 = static_cast<u32>(range);

            if (range32 == std::numeric_limits<u32>::max()) {
                // NOTE: Full 32-bit range; any value is valid.
                return static_cast<IntType>(
                    static_cast<UnsignedType>(a_) +
                    static_cast<UnsignedType>(static_cast<u32>(g() >> 32)));
            }

            const u32 s = range32 + u32{1};

            // NOTE: Nearly Divisionless method (Daniel Lemire, 2019).
            // Uses a single 32×32→64 multiply to map random values into [0, s).
            u32 x = static_cast<u32>(g() >> 32);
            u64 m = static_cast<u64>(x) * static_cast<u64>(s);
            u32 lo = static_cast<u32>(m);

            if (lo < s) {
                const u32 threshold = (u32{0} - s) % s;
                while (lo < threshold) {
                    x = static_cast<u32>(g() >> 32);
                    m = static_cast<u64>(x) * static_cast<u64>(s);
                    lo = static_cast<u32>(m);
                }
            }

            const u32 hi = static_cast<u32>(m >> 32);
            return static_cast<IntType>(
                static_cast<UnsignedType>(a_) +
                static_cast<UnsignedType>(hi));
        }
        else {
            // NOTE: 64-bit path using 128-bit multiplication.
            if (range == std::numeric_limits<u64>::max()) {
                // NOTE: The range spans the full u64 space.
                return static_cast<IntType>(
                    static_cast<UnsignedType>(a_) +
                    static_cast<UnsignedType>(g()));
            }

            const u64 s = range + u64{1};

            // NOTE: Nearly Divisionless method (Daniel Lemire, 2019).
            // Uses 128-bit multiplication to map random values into [0, s)
            // while avoiding modulo bias. The expensive modulo is computed
            // only when lo < s, which occurs with probability ~ s / 2^64.
            u64 x = g();
            u64 hi, lo;
            mul128_impl(x, s, hi, lo);

            if (lo < s) {
                const u64 threshold = (u64{0} - s) % s;
                while (lo < threshold) {
                    x = g();
                    mul128_impl(x, s, hi, lo);
                }
            }

            return static_cast<IntType>(
                static_cast<UnsignedType>(a_) +
                static_cast<UnsignedType>(hi));
        }
    }

    /// Returns the minimum value of the distribution.
    [[nodiscard]] result_type
    min() const noexcept
    {
        return a_;
    }

    /// Returns the maximum value of the distribution.
    [[nodiscard]] result_type
    max() const noexcept
    {
        return b_;
    }

private:
    /// Computes x * y as a 128-bit product, storing the high and low 64 bits.
    void mul128_impl(u64 x, u64 y, u64& hi, u64& lo) noexcept
    {
#if defined(_MSC_VER) && defined(_M_AMD64)
        // NOTE: x64 MSVC - _umul128 maps to a single MUL instruction.
        lo = _umul128(x, y, &hi);
#elif defined(_MSC_VER) && defined(_M_ARM64)
        // NOTE: ARM64 MSVC - __umulh maps to the UMULH instruction.
        hi = __umulh(x, y);
        lo = x * y;
#elif defined(__SIZEOF_INT128__)
        // NOTE: GCC/Clang with __uint128_t support (x86_64, ARM64, etc.)
        __uint128_t m = static_cast<__uint128_t>(x) * y;
        hi = static_cast<u64>(m >> 64);
        lo = static_cast<u64>(m);
#else
        // NOTE: Portable fallback using four 32x32→64 multiplications.
        // Decomposes x and y into 32-bit halves and reconstructs the 128-bit product:
        // ```
        // x * y = (x_hi * 2^32 + x_lo) * (y_hi * 2^32 + y_lo)
        // ```
        const u64 x_lo = x & UINT64_C(0xFFFFFFFF);
        const u64 x_hi = x >> 32;
        const u64 y_lo = y & UINT64_C(0xFFFFFFFF);
        const u64 y_hi = y >> 32;

        const u64 lo_lo = x_lo * y_lo;
        const u64 hi_lo = x_hi * y_lo;
        const u64 lo_hi = x_lo * y_hi;
        const u64 hi_hi = x_hi * y_hi;

        const u64 cross = (lo_lo >> 32) + (hi_lo & UINT64_C(0xFFFFFFFF)) + lo_hi;
        hi = hi_hi + (hi_lo >> 32) + (cross >> 32);
        lo = (cross << 32) | (lo_lo & UINT64_C(0xFFFFFFFF));
#endif
    }
};

} // namespace pomdog::random
