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

#include <array>
#include <Pomdog/Math/Vector4.hpp>

namespace Pomdog {

//class SkinnedVertex {
//public:
//	Vector2 Position;
//	Vector2 UV;
//	std::array<float, 3> Weights;
//	std::array<std::uint8_t, 4> Joints;
//};

class alignas(16) SkinnedVertex {
public:
	Vector4 PositionTextureCoord;
	std::array<float, 4> Weights;
	std::array<std::int32_t, 4> Joints;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SKINNEDVERTEX_181A0BE4_93EE_47D0_9239_2EC75635A335_HPP)
