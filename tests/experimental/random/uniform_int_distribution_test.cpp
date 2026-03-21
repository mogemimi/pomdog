// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/experimental/random/uniform_int_distribution.h"
#include "pomdog/experimental/random/xoroshiro128_star_star.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <limits>
#include <span>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::f64;
using pomdog::i32;
using pomdog::i64;
using pomdog::i8;
using pomdog::u32;
using pomdog::u64;
using pomdog::u8;
using pomdog::random::UniformIntDistribution;
using pomdog::random::Xoroshiro128StarStar;

/// Mock generator that returns pre-configured u64 values in sequence.
/// Satisfies the Generator requirements for UniformIntDistribution.
struct MockGenerator {
    using result_type = u64;
    static constexpr u64 min() noexcept { return 0; }
    static constexpr u64 max() noexcept { return std::numeric_limits<u64>::max(); }

    std::span<const u64> values = {};
    u32 index = 0;
    u32 callCount = 0;

    u64 operator()() noexcept
    {
        callCount++;
        return values[(index++) % values.size()];
    }
};

TEST_CASE("UniformIntDistribution")
{
    SUBCASE("min/max accessors")
    {
        UniformIntDistribution<i32> dist(10, 20);
        REQUIRE(dist.min() == 10);
        REQUIRE(dist.max() == 20);
    }

    SUBCASE("default constructor")
    {
        UniformIntDistribution<i32> dist;
        REQUIRE(dist.min() == 0);
        REQUIRE(dist.max() == std::numeric_limits<i32>::max());
    }

    SUBCASE("single value range")
    {
        Xoroshiro128StarStar rng(42);
        UniformIntDistribution<i32> dist(42, 42);
        for (i32 i = 0; i < 100; i++) {
            REQUIRE(dist(rng) == 42);
        }
    }

    SUBCASE("range [1, 6]")
    {
        Xoroshiro128StarStar rng(42);
        UniformIntDistribution<i32> dist(1, 6);
        for (i32 i = 0; i < 10000; i++) {
            const auto x = dist(rng);
            REQUIRE(x >= 1);
            REQUIRE(x <= 6);
        }
    }

    SUBCASE("range [-100, 100]")
    {
        Xoroshiro128StarStar rng(42);
        UniformIntDistribution<i32> dist(-100, 100);
        for (i32 i = 0; i < 10000; i++) {
            const auto x = dist(rng);
            REQUIRE(x >= -100);
            REQUIRE(x <= 100);
        }
    }

    SUBCASE("range [2, 254] with u8")
    {
        Xoroshiro128StarStar rng(42);
        UniformIntDistribution<u8> dist(2, 254);
        for (i32 i = 0; i < 1000; i++) {
            const auto x = dist(rng);
            REQUIRE(x >= 2);
            REQUIRE(x <= 254);
        }
    }

    SUBCASE("range [-127, 127] with i8")
    {
        Xoroshiro128StarStar rng(42);
        UniformIntDistribution<i8> dist(-127, 127);
        for (i32 i = 0; i < 1000; i++) {
            const auto x = dist(rng);
            REQUIRE(x >= -127);
            REQUIRE(x <= 127);
        }
    }

    SUBCASE("u64 full range")
    {
        Xoroshiro128StarStar rng(42);
        UniformIntDistribution<u64> dist(0, std::numeric_limits<u64>::max());
        for (i32 i = 0; i < 100; i++) {
            [[maybe_unused]] auto x = dist(rng);
        }
    }

    SUBCASE("i64 full range")
    {
        Xoroshiro128StarStar rng(42);
        UniformIntDistribution<pomdog::i64> dist(
            std::numeric_limits<pomdog::i64>::min(),
            std::numeric_limits<pomdog::i64>::max());
        for (int i = 0; i < 100; i++) {
            [[maybe_unused]] auto x = dist(rng);
        }
    }

    SUBCASE("deterministic output, seed=10000, range [1, 100]")
    {
        // NOTE: Verifies cross-platform determinism: the same seed + range must
        //       produce the exact same sequence on every platform.
        Xoroshiro128StarStar rng(10000);
        UniformIntDistribution<i32> dist(1, 100);

        REQUIRE(dist(rng) == 58);
        REQUIRE(dist(rng) == 80);
        REQUIRE(dist(rng) == 97);
        REQUIRE(dist(rng) == 41);
        REQUIRE(dist(rng) == 11);
        REQUIRE(dist(rng) == 25);
        REQUIRE(dist(rng) == 16);
        REQUIRE(dist(rng) == 6);
        REQUIRE(dist(rng) == 84);
        REQUIRE(dist(rng) == 13);
        REQUIRE(dist(rng) == 74);
        REQUIRE(dist(rng) == 24);
        REQUIRE(dist(rng) == 91);
        REQUIRE(dist(rng) == 91);
        REQUIRE(dist(rng) == 79);
        REQUIRE(dist(rng) == 94);
        REQUIRE(dist(rng) == 41);
        REQUIRE(dist(rng) == 85);
        REQUIRE(dist(rng) == 64);
        REQUIRE(dist(rng) == 82);
    }

    SUBCASE("deterministic output, seed=42, range [0, 9]")
    {
        Xoroshiro128StarStar rng(42);
        UniformIntDistribution<i32> dist(0, 9);

        REQUIRE(dist(rng) == 4);
        REQUIRE(dist(rng) == 2);
        REQUIRE(dist(rng) == 2);
        REQUIRE(dist(rng) == 0);
        REQUIRE(dist(rng) == 0);
        REQUIRE(dist(rng) == 1);
        REQUIRE(dist(rng) == 6);
        REQUIRE(dist(rng) == 1);
        REQUIRE(dist(rng) == 7);
        REQUIRE(dist(rng) == 1);
        REQUIRE(dist(rng) == 6);
        REQUIRE(dist(rng) == 1);
        REQUIRE(dist(rng) == 2);
        REQUIRE(dist(rng) == 8);
        REQUIRE(dist(rng) == 0);
        REQUIRE(dist(rng) == 6);
        REQUIRE(dist(rng) == 0);
        REQUIRE(dist(rng) == 0);
        REQUIRE(dist(rng) == 2);
        REQUIRE(dist(rng) == 9);
    }

    SUBCASE("uniformity [0, 9]")
    {
        constexpr i32 lo = 0;
        constexpr i32 hi = 9;
        constexpr u32 bucketCount = static_cast<u32>(hi - lo + 1);
        constexpr i32 n = 100000;

        Xoroshiro128StarStar rng(12345);
        UniformIntDistribution<i32> dist(lo, hi);
        std::vector<i32> counts(bucketCount, 0);
        for (i32 i = 0; i < n; i++) {
            const auto x = dist(rng);
            counts[static_cast<u32>(x - lo)]++;
        }

        const f64 expected = static_cast<f64>(n) / static_cast<f64>(bucketCount);
        for (u32 i = 0; i < bucketCount; i++) {
            // NOTE: Allow 10% deviation.
            REQUIRE(counts[i] > expected * 0.90);
            REQUIRE(counts[i] < expected * 1.10);
        }
    }

    SUBCASE("uniformity [1, 6]")
    {
        constexpr i32 lo = 1;
        constexpr i32 hi = 6;
        constexpr u32 bucketCount = static_cast<u32>(hi - lo + 1);
        constexpr i32 n = 100'000;

        Xoroshiro128StarStar rng(99999);
        UniformIntDistribution<i32> dist(lo, hi);
        std::vector<i32> counts(bucketCount, 0);
        for (i32 i = 0; i < n; i++) {
            const auto x = dist(rng);
            counts[static_cast<u32>(x - lo)]++;
        }

        const f64 expected = static_cast<f64>(n) / static_cast<f64>(bucketCount);
        for (u32 i = 0; i < bucketCount; i++) {
            // NOTE: Allow 10% deviation.
            REQUIRE(counts[i] > expected * 0.90);
            REQUIRE(counts[i] < expected * 1.10);
        }
    }

    SUBCASE("range [-1, 0] with i8")
    {
        // NOTE: This exercises the integer promotion edge case described
        //       in the following code's comment: uint8_t(0) - uint8_t(255) is promoted to int,
        //       yielding -255 without the extra cast back to UnsignedType.
        //       ```cpp
        //       const u64 range = static_cast<u64>(static_cast<UnsignedType>(
        //           static_cast<UnsignedType>(b_) - static_cast<UnsignedType>(a_)));
        //       ```
        Xoroshiro128StarStar rng(42);
        UniformIntDistribution<i8> dist(-1, 0);
        for (i32 i = 0; i < 1000; i++) {
            const auto x = dist(rng);
            REQUIRE(x >= -1);
            REQUIRE(x <= 0);
        }
    }

    SUBCASE("chi-squared uniformity [1, 100]")
    {
        // NOTE: Uses chi-squared goodness-of-fit to verify unbiased distribution.
        //       Critical value for chi-squared(99) at p=0.001 is 148.230.
        // FIXME: This is a relatively weak statistical test and only serves as a
        //        basic sanity check for distribution bias. It cannot detect more
        //        subtle issues such as correlations or structural patterns.
        //        For thorough evaluation, consider using dedicated test suites
        //        like TestU01 or PractRand.
        constexpr i32 lo = 1;
        constexpr i32 hi = 100;
        constexpr u32 bucketCount = static_cast<u32>(hi - lo + 1);
        constexpr i32 n = 100'000;

        Xoroshiro128StarStar rng(10000);
        UniformIntDistribution<i32> dist(lo, hi);
        std::vector<i32> counts(bucketCount, 0);
        for (i32 i = 0; i < n; i++) {
            const auto x = dist(rng);
            REQUIRE(x >= lo);
            REQUIRE(x <= hi);
            counts[static_cast<u32>(x - lo)]++;
        }

        const f64 expected = static_cast<f64>(n) / static_cast<f64>(bucketCount);
        f64 chi2 = 0.0;
        for (u32 i = 0; i < bucketCount; i++) {
            const f64 diff = static_cast<f64>(counts[i]) - expected;
            chi2 += (diff * diff) / expected;
        }
        REQUIRE(chi2 < 148.230);
    }

    SUBCASE("range == 0 for various types")
    {
        // NOTE: When `a == b`, `g()` should never be called.
        constexpr u64 values[] = {0};
        {
            MockGenerator gen{.values = values};
            UniformIntDistribution<u32> dist(42, 42);
            REQUIRE(dist(gen) == 42);
            REQUIRE(gen.callCount == 0);
        }
        {
            MockGenerator gen{.values = values};
            UniformIntDistribution<i32> dist(-7, -7);
            REQUIRE(dist(gen) == -7);
            REQUIRE(gen.callCount == 0);
        }
        {
            MockGenerator gen{.values = values};
            UniformIntDistribution<u64> dist(12345, 12345);
            REQUIRE(dist(gen) == 12345);
            REQUIRE(gen.callCount == 0);
        }
        {
            MockGenerator gen{.values = values};
            UniformIntDistribution<i64> dist(-999, -999);
            REQUIRE(dist(gen) == -999);
            REQUIRE(gen.callCount == 0);
        }
    }

    SUBCASE("single call when lo >= s")
    {
        // NOTE: 32-bit path. dist [0, 9], s = 10.
        //       x = 1, m = 10, lo = 10 >= s(10). One call to `g()`.
        {
            constexpr u64 values[] = {u64{1} << 32};
            MockGenerator gen{.values = values};
            UniformIntDistribution<i32> dist(0, 9);
            const auto result = dist(gen);
            REQUIRE(gen.callCount == 1);
            REQUIRE(result == 0);
        }
        // NOTE: 64-bit path. dist [0, 9], s = 10.
        //       x = 1, hi = 0, lo = 10 >= s(10). One call to `g()`.
        {
            constexpr u64 values[] = {1};
            MockGenerator gen{.values = values};
            UniformIntDistribution<i64> dist(0, 9);
            const auto result = dist(gen);
            REQUIRE(gen.callCount == 1);
            REQUIRE(result == 0);
        }
    }

    SUBCASE("rejection loop when lo < threshold")
    {
        // NOTE: 32-bit path. dist [0, 6], s = 7, threshold = 4.
        //       1st call: x = 0, lo = 0 < threshold(4) -> reject.
        //       2nd call: x = 1, lo = 7 >= threshold(4) -> accept. hi = 0.
        {
            constexpr u64 values[] = {u64{0}, u64{1} << 32};
            MockGenerator gen{.values = values};
            UniformIntDistribution<i32> dist(0, 6);
            const auto result = dist(gen);
            REQUIRE(gen.callCount == 2);
            REQUIRE(result == 0);
        }
        // NOTE: 64-bit path. dist [0, 6], s = 7, threshold = 2.
        //       1st call: x = 0, lo = 0 < threshold(2) -> reject.
        //       2nd call: x = 1, lo = 7 >= threshold(2) -> accept. hi = 0.
        {
            constexpr u64 values[] = {u64{0}, u64{1}};
            MockGenerator gen{.values = values};
            UniformIntDistribution<i64> dist(0, 6);
            const auto result = dist(gen);
            REQUIRE(gen.callCount == 2);
            REQUIRE(result == 0);
        }
    }

    SUBCASE("full range with signed types")
    {
        // NOTE: i32 full range. Verifies that a_ offset is applied
        //       so that g()=0 maps to i32::min, not 0.
        {
            constexpr u64 values[] = {0};
            MockGenerator gen{.values = values};
            UniformIntDistribution<i32> dist(
                std::numeric_limits<i32>::min(),
                std::numeric_limits<i32>::max());
            REQUIRE(dist(gen) == std::numeric_limits<i32>::min());
        }
        {
            constexpr u64 values[] = {u64{0xFFFFFFFF} << 32};
            MockGenerator gen{.values = values};
            UniformIntDistribution<i32> dist(
                std::numeric_limits<i32>::min(),
                std::numeric_limits<i32>::max());
            REQUIRE(dist(gen) == std::numeric_limits<i32>::max());
        }
        // NOTE: i64 full range.
        {
            constexpr u64 values[] = {0};
            MockGenerator gen{.values = values};
            UniformIntDistribution<i64> dist(
                std::numeric_limits<i64>::min(),
                std::numeric_limits<i64>::max());
            REQUIRE(dist(gen) == std::numeric_limits<i64>::min());
        }
        {
            constexpr u64 values[] = {std::numeric_limits<u64>::max()};
            MockGenerator gen{.values = values};
            UniformIntDistribution<i64> dist(
                std::numeric_limits<i64>::min(),
                std::numeric_limits<i64>::max());
            REQUIRE(dist(gen) == std::numeric_limits<i64>::max());
        }
    }

    SUBCASE("edge values: min and max")
    {
        // NOTE: 32-bit path, dist [10, 20], s = 11.
        //       x = 1, hi = 0 -> result = 10 (min).
        {
            constexpr u64 values[] = {u64{1} << 32};
            MockGenerator gen{.values = values};
            UniformIntDistribution<i32> dist(10, 20);
            REQUIRE(dist(gen) == 10);
        }
        // NOTE: x = 0xFFFFFFFF, hi = 10 -> result = 10 + 10 = 20 (max).
        {
            constexpr u64 values[] = {UINT64_C(0xFFFFFFFF) << 32};
            MockGenerator gen{.values = values};
            UniformIntDistribution<i32> dist(10, 20);
            REQUIRE(dist(gen) == 20);
        }
        // NOTE: 64-bit path, dist [10, 20], s = 11.
        //       x = 1, hi = 0 -> result = 10 (min).
        {
            constexpr u64 values[] = {1};
            MockGenerator gen{.values = values};
            UniformIntDistribution<i64> dist(10, 20);
            REQUIRE(dist(gen) == 10);
        }
        // NOTE: x = UINT64_MAX, hi = 10 -> result = 10 + 10 = 20 (max).
        {
            constexpr u64 values[] = {std::numeric_limits<u64>::max()};
            MockGenerator gen{.values = values};
            UniformIntDistribution<i64> dist(10, 20);
            REQUIRE(dist(gen) == 20);
        }
        // NOTE: u32 full range.
        {
            constexpr u64 values[] = {0};
            MockGenerator gen{.values = values};
            UniformIntDistribution<u32> dist(0, std::numeric_limits<u32>::max());
            REQUIRE(dist(gen) == 0);
        }
        {
            constexpr u64 values[] = {UINT64_C(0xFFFFFFFF) << 32};
            MockGenerator gen{.values = values};
            UniformIntDistribution<u32> dist(0, std::numeric_limits<u32>::max());
            REQUIRE(dist(gen) == std::numeric_limits<u32>::max());
        }
        // NOTE: u64 full range.
        {
            constexpr u64 values[] = {0};
            MockGenerator gen{.values = values};
            UniformIntDistribution<u64> dist(0, std::numeric_limits<u64>::max());
            REQUIRE(dist(gen) == 0);
        }
        {
            constexpr u64 values[] = {std::numeric_limits<u64>::max()};
            MockGenerator gen{.values = values};
            UniformIntDistribution<u64> dist(0, std::numeric_limits<u64>::max());
            REQUIRE(dist(gen) == std::numeric_limits<u64>::max());
        }
    }
}
