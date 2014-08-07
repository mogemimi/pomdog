//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SKINNEDVERTEX_181A0BE4_93EE_47D0_9239_2EC75635A335_HPP
#define POMDOG_SKINNEDVERTEX_181A0BE4_93EE_47D0_9239_2EC75635A335_HPP

#if (_MSC_VER > 1000)
#	pragma once
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
