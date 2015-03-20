// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_MAKEFOURCC_ED6FB253_174A_46D4_9EDA_D3ADDDAB06C2_HPP
#define POMDOG_MAKEFOURCC_ED6FB253_174A_46D4_9EDA_D3ADDDAB06C2_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>
#include <type_traits>

namespace Pomdog {
namespace Detail {

template <typename Character>
constexpr std::uint32_t MakeFourCC(Character c0, Character c1, Character c2, Character c3)
{
	static_assert(std::is_integral<Character>::value, "");
	static_assert(sizeof(Character) == 1, "");

	// FourCC's byte order is little endian.
	return (static_cast<std::uint32_t>(c0))
		| (static_cast<std::uint32_t>(c1) << 8)
		| (static_cast<std::uint32_t>(c2) << 16)
		| (static_cast<std::uint32_t>(c3) << 24);
}

}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_MAKEFOURCC_ED6FB253_174A_46D4_9EDA_D3ADDDAB06C2_HPP)
