// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/skeletal2d/animation_time_interval.hpp"
#include "pomdog/experimental/skeletal2d/tracks/animation_track.hpp"
#include "pomdog/math/rectangle.hpp"
#include "pomdog/math/vector2.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <optional>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d::detail {

struct SpriteKeyframe final {
    Rectangle Subrect;
    Vector2 Origin;
    AnimationTimeInterval Time;
    std::int16_t TexturePage;
    bool TextureRotate;
};

class SpriteAnimationTrack final : public AnimationTrack {
public:
    SpriteAnimationTrack() = default;
    SpriteAnimationTrack(std::vector<SpriteKeyframe>&& keys, std::uint32_t slotHashID);

    void Apply(
        const AnimationTimeInterval& time,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin) override;

    AnimationTimeInterval GetLength() const override;

private:
    std::vector<SpriteKeyframe> keys;
    std::uint32_t slotHashID;
    std::optional<std::uint16_t> slotIndex;
};

} // namespace pomdog::skeletal2d::detail
