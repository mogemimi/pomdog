// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/texture_atlas/texture_atlas.h"
#include "pomdog/math/vector2.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string_view>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {
class SkeletonPose;
class SkinnedMesh;
} // namespace pomdog::skeletal2d

namespace pomdog::spine {

class SkinData;

/// Creates a skinned mesh of the skin with the given name from a
/// `.skin_skel2d` file. A slot that shows a region becomes a quad.
///
/// `bindPosesInGlobal` has a matrix for each joint of the skeleton, and
/// `textureAtlas` must have a region for every attachment that the slots show.
[[nodiscard]] std::tuple<skeletal2d::SkinnedMesh, std::unique_ptr<Error>>
CreateSkinnedMesh(
    const std::vector<Matrix3x2>& bindPosesInGlobal,
    const SkinData& skinData,
    const TextureAtlas& textureAtlas,
    const Vector2& textureSize,
    std::string_view skinName);

} // namespace pomdog::spine
