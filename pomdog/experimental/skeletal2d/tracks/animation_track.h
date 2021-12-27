// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/experimental/skeletal2d/animation_time_interval.h"

namespace pomdog::skeletal2d {

class Skeleton;
class SkeletonPose;
class Skin;

class AnimationTrack {
public:
    virtual ~AnimationTrack();

    virtual void Apply(
        const AnimationTimeInterval& time,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin) = 0;

    virtual AnimationTimeInterval GetLength() const = 0;
};

} // namespace pomdog::skeletal2d
