//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_RIGIDSLOT_4C507F41_9ED4_483A_A6C3_E3133BF6398D_HPP
#define POMDOG_RIGIDSLOT_4C507F41_9ED4_483A_A6C3_E3133BF6398D_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include <Pomdog/Math/Rectangle.hpp>
#include <Pomdog/Math/Vector2.hpp>
#include <Pomdog/Math/Color.hpp>
#include <Pomdog/Math/Radian.hpp>
#include "JointIndex.hpp"

namespace Pomdog {

class RigidSlot {
public:
	Rectangle Subrect;
	Vector2 Translate;
	Vector2 Scale;
	Radian<float> Rotation;
	Color Color;
	Vector2 Origin;
	JointIndex JointIndex;
	std::uint32_t HashId;
	std::uint16_t DrawOrder;
	std::uint16_t TexturePage;
	bool TextureRotate;
};

}// namespace Pomdog

#endif // !defined(POMDOG_RIGIDSLOT_4C507F41_9ED4_483A_A6C3_E3133BF6398D_HPP)
