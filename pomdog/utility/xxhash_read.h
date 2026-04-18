// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::hash::detail {

[[nodiscard]] inline constexpr u8
read_u8(const char* input, size_t pos) noexcept
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    return static_cast<u8>(input[pos]);
    POMDOG_CLANG_UNSAFE_BUFFER_END
}

[[nodiscard]] inline constexpr u32
read_u32le(const char* input, size_t pos) noexcept
{
    const u32 b0 = read_u8(input, pos + 0);
    const u32 b1 = read_u8(input, pos + 1);
    const u32 b2 = read_u8(input, pos + 2);
    const u32 b3 = read_u8(input, pos + 3);
    return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
}

[[nodiscard]] inline constexpr u64
read_u64le(const char* input, size_t pos) noexcept
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
