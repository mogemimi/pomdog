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
#include "SkeletonDesc.hpp"
#include "TextureAtlas.hpp"

namespace Pomdog {

class ModelSlot2D {
public:
	Rectangle Subrect;
	Vector2 Translate;
	Vector2 Scale;
	Radian<float> Rotation;
	Color Color;
	Vector2 Origin;
	std::uint16_t DrawOrder;
	std::uint16_t BoneIndex;
	std::uint16_t TexturePage;
	bool TextureRotate;
};

class Skin {
public:
	Skin() = default;
	explicit Skin(std::vector<ModelSlot2D> && slots);

	std::vector<ModelSlot2D> const& Slots() const;

private:
	std::vector<ModelSlot2D> slots;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SKIN_8B9C736F_D6E4_449A_A310_1F791AA6ABC5_HPP)
