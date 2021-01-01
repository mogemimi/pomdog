// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/TexturePacker/TextureAtlas.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
