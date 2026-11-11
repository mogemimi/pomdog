// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/texture_atlas/texture_atlas.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string_view>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {

class AnimationClip;
class AnimationClipData;

/// Creates the animation clip with the given name from a `.clip_skel2d` file.
///
/// The clip gets the tracks that switch the attachments of slots only when
/// `textureAtlas` is not null, which must then have a region for every
/// attachment that those tracks show.
[[nodiscard]] std::tuple<std::shared_ptr<AnimationClip>, std::unique_ptr<Error>>
createAnimationClip(
    const AnimationClipData& clipData,
    const std::shared_ptr<TextureAtlas>& textureAtlas,
    std::string_view name);

/// Creates the animation clip whose name has the given xxHash32 key from a
/// `.clip_skel2d` file, as `createAnimationClip()` does for a name.
[[nodiscard]] std::tuple<std::shared_ptr<AnimationClip>, std::unique_ptr<Error>>
createAnimationClipByKey(
    const AnimationClipData& clipData,
    const std::shared_ptr<TextureAtlas>& textureAtlas,
    u32 clipKey);

} // namespace pomdog::skeletal2d
