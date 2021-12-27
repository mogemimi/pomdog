// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/texture_packer/texture_atlas.h"
#include "pomdog/math/vector2.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {
class SkeletonPose;
class SkinnedMesh;
} // namespace pomdog::skeletal2d

namespace pomdog::spine {

class SkeletonDesc;

[[nodiscard]] std::tuple<skeletal2d::SkinnedMesh, std::unique_ptr<Error>>
CreateSkinnedMesh(
    const std::vector<Matrix3x2>& bindPosesInGlobal,
    const SkeletonDesc& skeletonDesc,
    const TexturePacker::TextureAtlas& textureAtlas,
    const Vector2& textureSize,
    const std::string& skinName);

} // namespace pomdog::spine
