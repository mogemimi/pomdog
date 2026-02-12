// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/random/xoroshiro128_star_star.h"

namespace pomdog::random {
namespace {

[[nodiscard]] u64 rotl(const u64 x, int k) noexcept
{
    return (x << k) | (x >> (64 - k));
}

} // namespace

Xoroshiro128StarStar::Xoroshiro128StarStar() noexcept
{
    s[0] = 10000;
    s[1] = 0;
}

Xoroshiro128StarStar::Xoroshiro128StarStar(u64 seed) noexcept
{
    s[0] = seed;
    s[1] = 0;
}

u64 Xoroshiro128StarStar::next() noexcept
{
    // NOTE: Using Xoshiro128**. Please see the following pages
    //       http://prng.di.unimi.it/
    //       http://prng.di.unimi.it/xoroshiro128starstar.c

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
