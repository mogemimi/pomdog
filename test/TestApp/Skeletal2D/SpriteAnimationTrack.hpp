//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SPRITEANIMATIONTRACK_FC96976A_7E94_44C6_87DA_EC4C3C372204_HPP
#define POMDOG_SPRITEANIMATIONTRACK_FC96976A_7E94_44C6_87DA_EC4C3C372204_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <vector>
#include <Pomdog/Pomdog.hpp>
#include "Skin.hpp"

namespace Pomdog {
namespace Details {
namespace Skeletal2D {

class SpriteKeyframe {
public:
	Rectangle Subrect;
	Vector2 Origin;
	float TimeSeconds;
	std::uint16_t TexturePage;
	bool TextureRotate;
};

class SpriteAnimationTrack final {
public:
	SpriteAnimationTrack() = default;
	SpriteAnimationTrack(std::vector<SpriteKeyframe> && keys, std::uint16_t slotIndex);

	void Apply(Skin & skin, DurationSeconds const& time);
	
	DurationSeconds Length() const;
	
private:
	std::vector<SpriteKeyframe> keys;
	std::uint16_t slotIndex;
};

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SPRITEANIMATIONTRACK_FC96976A_7E94_44C6_87DA_EC4C3C372204_HPP)
