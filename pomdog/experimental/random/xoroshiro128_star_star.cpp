// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/random/xoroshiro128_star_star.h"

namespace pomdog::random {
namespace {

[[nodiscard]] u64 rotl(const u64 x, int k) noexcept
{
    return (x << k) | (x >> (64 - k));
}

/// SplitMix64: expands a single u64 seed into independent state words
/// used to initialize the state of Xoroshiro128StarStar. This is not
/// a general-purpose generator and should only be used for seeding.
[[nodiscard]] u64 splitmix64(u64& x) noexcept
{
    // NOTE: Using SplitMix64. Please see the following pages
    //       https://prng.di.unimi.it/splitmix64.c
    x += UINT64_C(0x9e3779b97f4a7c15);
    u64 z = x;
    z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);
    return z ^ (z >> 31);
}

} // namespace

Xoroshiro128StarStar::Xoroshiro128StarStar() noexcept
{
    u64 seed = 10000;
    s[0] = splitmix64(seed);
    s[1] = splitmix64(seed);
    if (s[0] == 0 && s[1] == 0) {
        s[0] = 1;
    }
}

Xoroshiro128StarStar::Xoroshiro128StarStar(u64 seed) noexcept
{
    s[0] = splitmix64(seed);
    s[1] = splitmix64(seed);
    if (s[0] == 0 && s[1] == 0) {
        s[0] = 1;
    }
}

u64 Xoroshiro128StarStar::next() noexcept
{
    // NOTE: Using Xoshiro128**. Please see the following pages
    //       https://prng.di.unimi.it/
    //       https://prng.di.unimi.it/xoroshiro128starstar.c

    const u64 s0 = s[0];
    u64 s1 = s[1];
    const u64 result = rotl(s0 * 5, 7) * 9;

    s1 ^= s0;
    s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16);
    s[1] = rotl(s1, 37);

    return result;
}

void Xoroshiro128StarStar::jump() noexcept
{
    constexpr u64 jump[] = {UINT64_C(0xdf900294d8f554a5), UINT64_C(0x170865df4b3201fc)};
    constexpr int x = static_cast<int>(sizeof(jump) / sizeof(*jump));
    u64 s0 = 0;
    u64 s1 = 0;

    for (int i = 0; i < x; i++) {
        for (int b = 0; b < 64; b++) {
            if (jump[i] & UINT64_C(1) << b) {
                s0 ^= s[0];
                s1 ^= s[1];
            }
            next();
        }
    }

    s[0] = s0;
    s[1] = s1;
}

u64 Xoroshiro128StarStar::operator()() noexcept
{
    return next();
}

[[nodiscard]] std::tuple<u64, u64> Xoroshiro128StarStar::preserve() const noexcept
{
    return std::make_tuple(s[0], s[1]);
}

void Xoroshiro128StarStar::reconstitute(u64 s0, u64 s1) noexcept
{
    s[0] = s0;
    s[1] = s1;
}

} // namespace pomdog::random
