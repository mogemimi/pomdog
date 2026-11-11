// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/animation_time_interval.h"
#include "pomdog/experimental/skeletal2d/tracks/animation_track.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d::detail {

struct SpriteKeyframe final {
    Rect2D subrect;
    Vector2 origin;
    AnimationTimeInterval time;
    i16 texturePage;
    bool textureRotate;
};

class SpriteAnimationTrack final : public AnimationTrack {
public:
    SpriteAnimationTrack() = default;
    SpriteAnimationTrack(std::vector<SpriteKeyframe>&& keys, u32 slotHashID);

    void apply(
        const AnimationTimeInterval& time,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin) override;

    AnimationTimeInterval getLength() const override;

private:
    std::vector<SpriteKeyframe> keys_;
    u32 slotHashID_;
    std::optional<u16> slotIndex_;
};

} // namespace pomdog::skeletal2d::detail
