// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SPINE_SKINLOADER_E56D7A0C_6120_47CD_918E_5222C7BC98A3_HPP
#define POMDOG_SPINE_SKINLOADER_E56D7A0C_6120_47CD_918E_5222C7BC98A3_HPP

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
