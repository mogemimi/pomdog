//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SPRITEANIMATIONTRACK_FC96976A_7E94_44C6_87DA_EC4C3C372204_HPP
#define POMDOG_SPRITEANIMATIONTRACK_FC96976A_7E94_44C6_87DA_EC4C3C372204_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "AnimationTimeInterval.hpp"
#include "Skin.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <cstdint>
#include <vector>

namespace Pomdog {
namespace Details {
namespace Skeletal2D {

class SpriteKeyframe {
public:
	Rectangle Subrect;
	Vector2 Origin;
	AnimationTimeInterval Time;
	std::uint16_t TexturePage;
	bool TextureRotate;
};

class SpriteAnimationTrack final {
public:
	SpriteAnimationTrack() = default;
	SpriteAnimationTrack(std::vector<SpriteKeyframe> && keys, std::uint32_t slotHashId);

	void Apply(Skin & skin, AnimationTimeInterval const& time);

	AnimationTimeInterval Length() const;

private:
	std::vector<SpriteKeyframe> keys;
	std::uint32_t slotHashId;
	Optional<std::uint16_t> slotIndex;
};

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SPRITEANIMATIONTRACK_FC96976A_7E94_44C6_87DA_EC4C3C372204_HPP)
