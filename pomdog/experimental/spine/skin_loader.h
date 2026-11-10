// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/texture_atlas/texture_atlas.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string_view>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {
class Skin;
} // namespace pomdog::skeletal2d

namespace pomdog::spine {

class SkinData;

/// Creates the skin with the given name from a `.skin_skel2d` file, as sprites
/// that follow joints.
///
/// The skin leaves out the slots that show meshes; use `CreateSkinnedMesh()`
/// to draw those. `textureAtlas` must have a region for every attachment that
/// the slots show.
[[nodiscard]] std::tuple<std::shared_ptr<skeletal2d::Skin>, std::unique_ptr<Error>>
CreateSkin(
    const SkinData& skinData,
    const TextureAtlas& textureAtlas,
    std::string_view skinName);

} // namespace pomdog::spine
