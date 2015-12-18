// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_SKINLOADER_E56D7A0C_HPP
#define POMDOG_SKINLOADER_E56D7A0C_HPP

#include "Pomdog.Experimental/TexturePacker/TextureAtlas.hpp"

namespace Pomdog {

class Skin;
class SkeletonDesc;

namespace Spine {

class SkeletonDesc;

Skin CreateSkin(SkeletonDesc const& skeletonDesc,
    TexturePacker::TextureAtlas const& textureAtlas,
    std::string const& skinName);

} // namespace Spine
} // namespace Pomdog

#endif // POMDOG_SKINLOADER_E56D7A0C_HPP
