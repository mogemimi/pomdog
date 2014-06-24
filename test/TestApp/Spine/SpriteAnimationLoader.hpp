//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SPRITEANIMATIONLOADER_C360BEBA_795B_404D_932B_CFD7ACF40805_HPP
#define POMDOG_SPRITEANIMATIONLOADER_C360BEBA_795B_404D_932B_CFD7ACF40805_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <vector>
#include "TextureAtlas.hpp"

namespace Pomdog {
namespace Details {
namespace Skeletal2D {

class SkeletonDesc;
class SpriteAnimationTrack;

std::vector<SpriteAnimationTrack> CreateSpriteAnimationTrack(
	SkeletonDesc const& desc,
	Details::TexturePacker::TextureAtlas const& textureAtlas,
	char const* name);

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SPRITEANIMATIONLOADER_C360BEBA_795B_404D_932B_CFD7ACF40805_HPP)
