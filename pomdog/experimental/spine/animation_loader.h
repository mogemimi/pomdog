// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/texture_packer/texture_atlas.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {
class AnimationClip;
} // namespace pomdog::skeletal2d

namespace pomdog::spine {

class SkeletonDesc;

[[nodiscard]] std::tuple<std::shared_ptr<skeletal2d::AnimationClip>, std::unique_ptr<Error>>
CreateAnimationClip(
    const SkeletonDesc& desc,
    const std::optional<TexturePacker::TextureAtlas>& textureAtlas,
    const std::string& name);

} // namespace pomdog::spine
