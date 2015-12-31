// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "AnimationTimeInterval.hpp"

namespace Pomdog {

class Skeleton;
class SkeletonPose;

class AnimationTrack {
public:
    virtual ~AnimationTrack() = default;

    virtual void Apply(AnimationTimeInterval const& time, Skeleton const& skeleton,
        SkeletonPose & skeletonPose) = 0;

    virtual AnimationTimeInterval Length() const = 0;
};

} // namespace Pomdog
