//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SKIN_8B9C736F_D6E4_449A_A310_1F791AA6ABC5_HPP
#define POMDOG_SKIN_8B9C736F_D6E4_449A_A310_1F791AA6ABC5_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include <Pomdog/Pomdog.hpp>
#include "JointIndex.hpp"
#include "SkeletonDesc.hpp"

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
	std::uint16_t DrawOrder;
	std::uint16_t TexturePage;
	bool TextureRotate;
};

class Skin {
public:
	Skin() = default;
	explicit Skin(std::vector<RigidSlot> && slots);

	std::vector<RigidSlot> const& Slots() const;
	
	RigidSlot const& Slots(std::uint16_t slotIndex) const;
	RigidSlot & Slots(std::uint16_t slotIndex);

private:
	std::vector<RigidSlot> slots;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SKIN_8B9C736F_D6E4_449A_A310_1F791AA6ABC5_HPP)
