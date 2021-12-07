// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/texture_packer/texture_atlas.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Skeletal2D {
class AnimationClip;
} // namespace Pomdog::Skeletal2D

namespace Pomdog::Spine {

class SkeletonDesc;

[[nodiscard]] std::tuple<std::shared_ptr<Skeletal2D::AnimationClip>, std::unique_ptr<Error>>
CreateAnimationClip(
    const SkeletonDesc& desc,
    const std::optional<TexturePacker::TextureAtlas>& textureAtlas,
    const std::string& name);

} // namespace Pomdog::Spine
