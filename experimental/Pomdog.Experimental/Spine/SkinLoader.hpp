// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/TexturePacker/TextureAtlas.hpp"

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
