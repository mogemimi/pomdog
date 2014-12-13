//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SPINE_SKINLOADER_E56D7A0C_6120_47CD_918E_5222C7BC98A3_HPP
#define POMDOG_SPINE_SKINLOADER_E56D7A0C_6120_47CD_918E_5222C7BC98A3_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Pomdog.Experimental/TexturePacker/TextureAtlas.hpp"

namespace Pomdog {

class Skin;
class SkeletonDesc;

namespace Spine {

class SkeletonDesc;

Skin CreateSkin(SkeletonDesc const& skeletonDesc,
	TexturePacker::TextureAtlas const& textureAtlas,
	std::string const& skinName);

}// namespace Spine
}// namespace Pomdog

#endif // !defined(POMDOG_SPINE_SKINLOADER_E56D7A0C_6120_47CD_918E_5222C7BC98A3_HPP)
