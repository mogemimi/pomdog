// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Skeletal2D/AnimationTimeInterval.hpp"
#include "Pomdog/Experimental/Skeletal2D/CompressedFloat.hpp"
#include "Pomdog/Experimental/Skeletal2D/JointIndex.hpp"
#include "Pomdog/Experimental/Skeletal2D/Tracks/AnimationTrack.hpp"
#include <vector>

namespace Pomdog::Skeletal2D::Detail {

struct ScaleKeyframe final {
    AnimationTimeInterval Time;
    CompressedFloat<std::int16_t, 1500> Scale;

    static_assert(decltype(Scale)::Max() > 20.0f, "");
    static_assert(decltype(Scale)::Min() < -20.0f, "");
};

class ScaleTrack final : public AnimationTrack {
public:
    ScaleTrack(std::vector<ScaleKeyframe>&& keys, JointIndex&& jointIndex);

    void Apply(
        const AnimationTimeInterval& time,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin) override;

    AnimationTimeInterval GetLength() const override;

private:
    std::vector<ScaleKeyframe> keys;
    JointIndex jointIndex;
};

} // namespace Pomdog::Skeletal2D::Detail
