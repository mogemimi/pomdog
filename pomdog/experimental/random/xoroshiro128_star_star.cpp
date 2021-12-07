// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/random/xoroshiro128_star_star.hpp"

namespace Pomdog::Random {
namespace {

std::uint64_t rotl(const std::uint64_t x, int k) noexcept
{
    return (x << k) | (x >> (64 - k));
}

} // namespace

Xoroshiro128StarStar::Xoroshiro128StarStar() noexcept
{
    s[0] = 10000;
    s[1] = 0;
}

Xoroshiro128StarStar::Xoroshiro128StarStar(std::uint64_t seed) noexcept
{
    s[0] = seed;
    s[1] = 0;
}

std::uint64_t Xoroshiro128StarStar::Next() noexcept
{
    // NOTE: Using Xoshiro128**. Please see the following pages
    // http://prng.di.unimi.it/
    // http://prng.di.unimi.it/xoroshiro128starstar.c

    const std::uint64_t s0 = s[0];
    std::uint64_t s1 = s[1];
    const std::uint64_t result = rotl(s0 * 5, 7) * 9;

    s1 ^= s0;
    s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16);
    s[1] = rotl(s1, 37);

    return result;
}

void Xoroshiro128StarStar::Jump() noexcept
{
    constexpr std::uint64_t jump[] = {0xdf900294d8f554a5, 0x170865df4b3201fc};
    constexpr int x = static_cast<int>(sizeof(jump) / sizeof(*jump));
    std::uint64_t s0 = 0;
    std::uint64_t s1 = 0;

    for (int i = 0; i < x; i++) {
        for (int b = 0; b < 64; b++) {
            if (jump[i] & UINT64_C(1) << b) {
                s0 ^= s[0];
                s1 ^= s[1];
            }
            Next();
        }
    }

    s[0] = s0;
    s[1] = s1;
}

std::uint64_t Xoroshiro128StarStar::operator()() noexcept
{
    return Next();
}

} // namespace Pomdog::Random
