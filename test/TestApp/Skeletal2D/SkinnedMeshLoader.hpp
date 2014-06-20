//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SKINNEDMESHLOADER_0D066D10_C749_4235_9B20_0F05627523E5_HPP
#define POMDOG_SKINNEDMESHLOADER_0D066D10_C749_4235_9B20_0F05627523E5_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include "TextureAtlas.hpp"

namespace Pomdog {

class SkinnedMesh;
class SkeletonPose;

namespace Details {
namespace Skeletal2D {

class SkeletonDesc;

SkinnedMesh CreateSkinnedMesh(
	std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	SkeletonPose const& bindPose,
	SkeletonDesc const& skeletonDesc,
	Details::TexturePacker::TextureAtlas const& textureAtlas,
	std::string const& skinName);

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SKINNEDMESHLOADER_0D066D10_C749_4235_9B20_0F05627523E5_HPP)
