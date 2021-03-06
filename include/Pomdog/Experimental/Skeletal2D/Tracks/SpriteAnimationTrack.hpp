// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/Skeletal2D/AnimationTimeInterval.hpp"
#include "Pomdog/Experimental/Skeletal2D/Tracks/AnimationTrack.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <optional>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Skeletal2D::Detail {

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

} // namespace Pomdog::Skeletal2D::Detail
