// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_BLENDFUNCTION_5A8D5F85_1D0C_4274_8115_BCEE3F6122EA_HPP
#define POMDOG_BLENDFUNCTION_5A8D5F85_1D0C_4274_8115_BCEE3F6122EA_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

enum class BlendFunction: std::uint8_t {
	Add,

	Subtract,

	ReverseSubtract,

	Min,

	Max,
};

}// namespace Pomdog

#endif // !defined(POMDOG_BLENDFUNCTION_5A8D5F85_1D0C_4274_8115_BCEE3F6122EA_HPP)
