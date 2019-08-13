// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>
#include <type_traits>

namespace Pomdog::Detail {

template <typename Character>
constexpr std::uint32_t MakeFourCC(Character c0, Character c1, Character c2, Character c3) noexcept
{
    static_assert(std::is_integral<Character>::value, "");
    static_assert(sizeof(Character) == 1, "");

    // FourCC's byte order is little endian.
    return (static_cast<std::uint32_t>(c0))
        | (static_cast<std::uint32_t>(c1) << 8)
        | (static_cast<std::uint32_t>(c2) << 16)
        | (static_cast<std::uint32_t>(c3) << 24);
}

} // namespace Pomdog::Detail
