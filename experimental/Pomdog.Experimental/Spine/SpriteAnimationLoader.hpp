//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SPINE_SPRITEANIMATIONLOADER_C360BEBA_795B_404D_932B_CFD7ACF40805_HPP
#define POMDOG_SPINE_SPRITEANIMATIONLOADER_C360BEBA_795B_404D_932B_CFD7ACF40805_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Pomdog.Experimental/TexturePacker/TextureAtlas.hpp"
#include <vector>

namespace Pomdog {
namespace Details {
namespace Skeletal2D {

class SpriteAnimationTrack;

}// namespace Skeletal2D
}// namespace Details
namespace Spine {

class SkeletonDesc;

std::vector<Details::Skeletal2D::SpriteAnimationTrack> CreateSpriteAnimationTrack(
	SkeletonDesc const& desc,
	TexturePacker::TextureAtlas const& textureAtlas,
	char const* name);

}// namespace Spine
}// namespace Pomdog

#endif // !defined(POMDOG_SPINE_SPRITEANIMATIONLOADER_C360BEBA_795B_404D_932B_CFD7ACF40805_HPP)
