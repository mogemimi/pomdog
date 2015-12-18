// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/TexturePacker/TextureAtlas.hpp"
#include <vector>
#include <string>

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

class SpriteAnimationTrack;

} // namespace Skeletal2D
} // namespace Detail
namespace Spine {

class SkeletonDesc;

std::vector<Detail::Skeletal2D::SpriteAnimationTrack> CreateSpriteAnimationTrack(
    SkeletonDesc const& desc,
    TexturePacker::TextureAtlas const& textureAtlas,
    std::string const& name);

} // namespace Spine
} // namespace Pomdog
