// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CULLMODE_DBEE9F5F_6FD8_4B0A_BED4_064A6635E957_HPP
#define POMDOG_CULLMODE_DBEE9F5F_6FD8_4B0A_BED4_064A6635E957_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

enum class CullMode: std::uint8_t {
	None,

	ClockwiseFace,

	CounterClockwiseFace,
};

}// namespace Pomdog

#endif // !defined(POMDOG_CULLMODE_DBEE9F5F_6FD8_4B0A_BED4_064A6635E957_HPP)
