// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "AnimationTrack.hpp"
#include "AnimationTimeInterval.hpp"
#include "CompressedFloat.hpp"
#include "JointIndex.hpp"
#include "Pomdog.Experimental/MSVCSupport.hpp"
#include <vector>

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

class ScaleKeyframe {
public:
    AnimationTimeInterval Time;
    CompressedFloat<std::int16_t, 1500> Scale;

#ifndef POMDOG_MSVC2015_SUPPORT
    static_assert(decltype(Scale)::Max() > 20.0f, "");
    static_assert(decltype(Scale)::Min() < -20.0f, "");
#endif
};

class ScaleTrack final: public AnimationTrack {
public:
    ScaleTrack(std::vector<ScaleKeyframe> && keys, JointIndex && jointIndex);

    void Apply(AnimationTimeInterval const& time, Skeleton const& skeleton,
        SkeletonPose & skeletonPose) override;

    AnimationTimeInterval Length() const override;

private:
    std::vector<ScaleKeyframe> keys;
    JointIndex jointIndex;
};

} // namespace Skeletal2D
} // namespace Detail
} // namespace Pomdog
