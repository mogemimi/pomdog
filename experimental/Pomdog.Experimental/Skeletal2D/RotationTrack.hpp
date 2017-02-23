// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "AnimationTrack.hpp"
#include "AnimationTimeInterval.hpp"
#include "CompressedFloat.hpp"
#include "JointIndex.hpp"
#include <vector>

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

struct RotationKeyframe {
    AnimationTimeInterval Time;
    CompressedFloat<std::int16_t, 4500> Rotation;

    static_assert(decltype(Rotation)::Max() > 3.1416f*2.0f, "");
    static_assert(decltype(Rotation)::Min() < -3.1416f*2.0f, "");
};

class RotationTrack final: public AnimationTrack {
public:
    RotationTrack(std::vector<RotationKeyframe> && keys, JointIndex && jointIndex);

    void Apply(
        AnimationTimeInterval const& time,
        Skeleton const& skeleton,
        SkeletonPose & skeletonPose) override;

    AnimationTimeInterval Length() const override;

private:
    std::vector<RotationKeyframe> keys;
    JointIndex jointIndex;
};

} // namespace Skeletal2D
} // namespace Detail
} // namespace Pomdog
