// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/TexturePacker/TextureAtlas.hpp"
#include <memory>
#include <string>

namespace Pomdog::Skeletal2D {
class Skin;
} // namespace Pomdog::Skeletal2D

namespace Pomdog::Spine {

class SkeletonDesc;

[[nodiscard]] std::shared_ptr<Skeletal2D::Skin>
CreateSkin(
    const SkeletonDesc& skeletonDesc,
    const TexturePacker::TextureAtlas& textureAtlas,
    const std::string& skinName);

} // namespace Pomdog::Spine
