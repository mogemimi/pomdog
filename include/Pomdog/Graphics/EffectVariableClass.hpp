// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTVARIABLECLASS_0B44E0FB_0B71_45D3_9981_8089B38B8655_HPP
#define POMDOG_EFFECTVARIABLECLASS_0B44E0FB_0B71_45D3_9981_8089B38B8655_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

enum class EffectVariableClass: std::uint8_t {
	Scalar,

	Vector,

	Matrix,

	Object,

	Struct,
};

}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTVARIABLECLASS_0B44E0FB_0B71_45D3_9981_8089B38B8655_HPP)
