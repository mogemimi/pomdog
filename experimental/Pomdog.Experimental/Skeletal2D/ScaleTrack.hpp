// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "AnimationTrack.hpp"
#include "AnimationTimeInterval.hpp"
#include "CompressedFloat.hpp"
#include "JointIndex.hpp"
#include <vector>

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

struct ScaleKeyframe {
    AnimationTimeInterval Time;
    CompressedFloat<std::int16_t, 1500> Scale;

    static_assert(decltype(Scale)::Max() > 20.0f, "");
    static_assert(decltype(Scale)::Min() < -20.0f, "");
};

class ScaleTrack final: public AnimationTrack {
public:
    ScaleTrack(std::vector<ScaleKeyframe> && keys, JointIndex && jointIndex);

    void Apply(
        AnimationTimeInterval const& time,
        Skeleton const& skeleton,
        SkeletonPose & skeletonPose) override;

    AnimationTimeInterval Length() const override;

private:
    std::vector<ScaleKeyframe> keys;
    JointIndex jointIndex;
};

} // namespace Skeletal2D
} // namespace Detail
} // namespace Pomdog
