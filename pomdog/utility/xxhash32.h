// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/xxhash_read.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <bit>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(4514)

namespace pomdog::hash::detail {

/// Computes the left rotation of a 32-bit unsigned integer `v` by `x` bits.
[[nodiscard]] inline constexpr u32
rotl32(u32 v, i32 x) noexcept
{
#if defined(_MSC_VER) && (_MSC_VER <= 1927)
    return ((v << x) | (v >> (32 - x)));
#else
    return std::rotl(v, x);
#endif
}

} // namespace pomdog::hash::detail

namespace pomdog::hash::detail::xxh32 {

// NOTE: https://github.com/Cyan4973/xxHash/blob/dev/doc/xxhash_spec.md

inline constexpr u32 prime1 = 0x9E3779B1U;
inline constexpr u32 prime2 = 0x85EBCA77U;
inline constexpr u32 prime3 = 0xC2B2AE3DU;
inline constexpr u32 prime4 = 0x27D4EB2FU;
inline constexpr u32 prime5 = 0x165667B1U;

[[nodiscard]] inline constexpr u32
avalanche(u32 h32) noexcept
{
    h32 ^= h32 >> 15;
    h32 *= prime2;
    h32 ^= h32 >> 13;
    h32 *= prime3;
    h32 ^= h32 >> 16;
    return h32;
}

POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(5045) // NOTE: Spectre mitigation
[[nodiscard]] inline constexpr u32
finalize(const char* input, size_t inputLen, size_t pos, u32 h32) noexcept
{
    while ((inputLen - pos) >= 4) {
        h32 += read_u32le(input, pos) * prime3;
        h32 = rotl32(h32, 17) * prime4;
        pos += 4;
    }
    while ((inputLen - pos) > 0) {
        h32 += read_u8(input, pos) * prime5;
        h32 = rotl32(h32, 11) * prime1;
        pos += 1;
    }
    return h32;
}
POMDOG_MSVC_SUPPRESS_WARNING_POP

[[nodiscard]] inline constexpr u32
digest(const char* input, size_t inputLen, size_t pos, u32 v1, u32 v2, u32 v3, u32 v4) noexcept
{
    u32 h32 = 0;
    if (inputLen >= 16) {
        h32 = rotl32(v1, 1) + rotl32(v2, 7) + rotl32(v3, 12) + rotl32(v4, 18);
    }
    else {
        h32 = v3 + prime5;
    }
    return avalanche(finalize(input, inputLen, pos, h32 + static_cast<u32>(inputLen)));
}

[[nodiscard]] inline constexpr u32
round(u32 acc, u32 input) noexcept
{
    return rotl32(acc + (input * prime2), 13) * prime1;
}

POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(5045) // NOTE: Spectre mitigation
[[nodiscard]] inline constexpr u32
xxh32(const char* input, size_t inputLen, u32 seed) noexcept
{
    u32 v1 = seed + prime1 + prime2;
    u32 v2 = seed + prime2;
    u32 v3 = seed;
    u32 v4 = seed - prime1;
    size_t pos = 0;
    while (pos + 16 <= inputLen) {
        v1 = round(v1, read_u32le(input, pos + 0 * 4));
        v2 = round(v2, read_u32le(input, pos + 1 * 4));
        v3 = round(v3, read_u32le(input, pos + 2 * 4));
        v4 = round(v4, read_u32le(input, pos + 3 * 4));
        pos += 16;
    }
    return digest(input, inputLen, pos, v1, v2, v3, v4);
}
POMDOG_MSVC_SUPPRESS_WARNING_POP

} // namespace pomdog::hash::detail::xxh32

namespace pomdog::hash {

/// Computes the 32-bit hash of the given input string using the xxHash algorithm
/// with the specified seed. The function supports compile-time evaluation.
[[nodiscard]] inline constexpr u32
xxh32(const char* input, size_t inputLen, u32 seed) noexcept
{
    return detail::xxh32::xxh32(input, inputLen, seed);
}

} // namespace pomdog::hash

POMDOG_MSVC_SUPPRESS_WARNING_POP
