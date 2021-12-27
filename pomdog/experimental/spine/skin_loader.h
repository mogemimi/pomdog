// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/texture_packer/texture_atlas.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {
class Skin;
} // namespace pomdog::skeletal2d

namespace pomdog::spine {

class SkeletonDesc;

[[nodiscard]] std::shared_ptr<skeletal2d::Skin>
CreateSkin(
    const SkeletonDesc& skeletonDesc,
    const TexturePacker::TextureAtlas& textureAtlas,
    const std::string& skinName);

} // namespace pomdog::spine
