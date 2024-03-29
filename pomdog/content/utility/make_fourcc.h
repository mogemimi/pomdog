// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

template <typename Character>
[[nodiscard]] constexpr std::uint32_t
makeFourCC(Character c0, Character c1, Character c2, Character c3) noexcept
{
    static_assert(std::is_integral<Character>::value, "");
    static_assert(sizeof(Character) == 1, "");

    // FourCC's byte order is little endian.
    return (static_cast<std::uint32_t>(c0)) |
           (static_cast<std::uint32_t>(c1) << 8) |
           (static_cast<std::uint32_t>(c2) << 16) |
           (static_cast<std::uint32_t>(c3) << 24);
}

} // namespace pomdog::detail
