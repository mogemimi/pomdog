// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/xxhash32.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <bit>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

#if defined(_MSC_VER)
POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(4514)
#endif

namespace pomdog::hash::detail {

/// Computes the left rotation of a 64-bit unsigned integer `v` by `x` bits.
[[nodiscard]] inline constexpr u64
rotl64(u64 v, i32 x) noexcept
{
#if defined(_MSC_VER) && (_MSC_VER <= 1927)
    return ((v << x) | (v >> (64 - x)));
#else
    return std::rotl(v, x);
#endif
}

[[nodiscard]] inline constexpr u64
read_u64le(const char* input, int pos) noexcept
{
    const u64 b0 = read_u8(input, pos + 0);
    const u64 b1 = read_u8(input, pos + 1);
    const u64 b2 = read_u8(input, pos + 2);
    const u64 b3 = read_u8(input, pos + 3);
    const u64 b4 = read_u8(input, pos + 4);
    const u64 b5 = read_u8(input, pos + 5);
    const u64 b6 = read_u8(input, pos + 6);
    const u64 b7 = read_u8(input, pos + 7);
    return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24) |
           (b4 << 32) | (b5 << 40) | (b6 << 48) | (b7 << 56);
}

} // namespace pomdog::hash::detail

namespace pomdog::hash::detail::xxh64 {

// NOTE: https://github.com/Cyan4973/xxHash/blob/dev/doc/xxhash_spec.md

inline constexpr u64 prime1 = 0x9E3779B185EBCA87ULL;
inline constexpr u64 prime2 = 0xC2B2AE3D27D4EB4FULL;
inline constexpr u64 prime3 = 0x165667B19E3779F9ULL;
inline constexpr u64 prime4 = 0x85EBCA77C2B2AE63ULL;
inline constexpr u64 prime5 = 0x27D4EB2F165667C5ULL;

[[nodiscard]] inline constexpr u64
avalanche(u64 h64) noexcept
{
    h64 ^= h64 >> 33;
    h64 *= prime2;
    h64 ^= h64 >> 29;
    h64 *= prime3;
    h64 ^= h64 >> 32;
    return h64;
}

POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(5045) // NOTE: Spectre mitigation
[[nodiscard]] inline constexpr u64
finalize(const char* input, int inputLen, int pos, u64 h64) noexcept
{
    while ((inputLen - pos) >= 8) {
        const u64 k1 = rotl64(read_u64le(input, pos) * prime2, 31) * prime1;
        h64 ^= k1;
        h64 = rotl64(h64, 27) * prime1 + prime4;
        pos += 8;
    }
    while ((inputLen - pos) >= 4) {
        h64 ^= static_cast<u64>(read_u32le(input, pos)) * prime1;
        h64 = rotl64(h64, 23) * prime2 + prime3;
        pos += 4;
    }
    while ((inputLen - pos) > 0) {
        h64 ^= static_cast<u64>(read_u8(input, pos)) * prime5;
        h64 = rotl64(h64, 11) * prime1;
        pos += 1;
    }
    return h64;
}
POMDOG_MSVC_SUPPRESS_WARNING_POP

[[nodiscard]] inline constexpr u64
mergeRound(u64 acc, u64 val) noexcept
{
    val *= prime2;
    val = rotl64(val, 31);
    val *= prime1;
    acc ^= val;
    acc = acc * prime1 + prime4;
    return acc;
}

[[nodiscard]] inline constexpr u64
digest(const char* input, int inputLen, int pos, u64 v1, u64 v2, u64 v3, u64 v4) noexcept
{
    u64 h64 = 0;
    if (inputLen >= 32) {
        h64 = rotl64(v1, 1) + rotl64(v2, 7) + rotl64(v3, 12) + rotl64(v4, 18);
        h64 = mergeRound(h64, v1);
        h64 = mergeRound(h64, v2);
        h64 = mergeRound(h64, v3);
        h64 = mergeRound(h64, v4);
    }
    else {
        h64 = v3 + prime5;
    }
    return avalanche(finalize(input, inputLen, pos, h64 + static_cast<u64>(inputLen)));
}

[[nodiscard]] inline constexpr u64
round(u64 acc, const char* input, int pos) noexcept
{
    const u64 d = read_u64le(input, pos);
    return rotl64(acc + (d * prime2), 31) * prime1;
}

POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(5045) // NOTE: Spectre mitigation
[[nodiscard]] inline constexpr u64
xxh64(const char* input, int inputLen, u64 seed) noexcept
{
    u64 v1 = seed + prime1 + prime2;
    u64 v2 = seed + prime2;
    u64 v3 = seed;
    u64 v4 = seed - prime1;
    int pos = 0;
    while (pos + 32 <= inputLen) {
        v1 = round(v1, input, pos + 0 * 8);
        v2 = round(v2, input, pos + 1 * 8);
        v3 = round(v3, input, pos + 2 * 8);
        v4 = round(v4, input, pos + 3 * 8);
        pos += 32;
    }
    return digest(input, inputLen, pos, v1, v2, v3, v4);
}
POMDOG_MSVC_SUPPRESS_WARNING_POP

} // namespace pomdog::hash::detail::xxh64

namespace pomdog::hash {

/// Computes the 64-bit hash of the given input string using the xxHash algorithm
/// with the specified seed. The function supports compile-time evaluation.
[[nodiscard]] inline constexpr u64
xxh64(const char* input, int inputLen, u64 seed) noexcept
{
    return detail::xxh64::xxh64(input, inputLen, seed);
}

} // namespace pomdog::hash

#if defined(_MSC_VER)
POMDOG_MSVC_SUPPRESS_WARNING_POP
#endif
