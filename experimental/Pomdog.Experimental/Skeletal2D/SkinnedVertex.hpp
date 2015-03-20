// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SKINNEDVERTEX_181A0BE4_93EE_47D0_9239_2EC75635A335_HPP
#define POMDOG_SKINNEDVERTEX_181A0BE4_93EE_47D0_9239_2EC75635A335_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Math/Vector4.hpp"
#include <cstdint>
#include <array>

namespace Pomdog {

class alignas(16) SkinnedVertex {
public:
	// {xy__} = position.xy
	// {__zw} = textureCoord.xy
	Vector4 PositionTextureCoord;

	std::array<float, 4> Weights;

	std::array<std::int32_t, 4> Joints;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SKINNEDVERTEX_181A0BE4_93EE_47D0_9239_2EC75635A335_HPP)
