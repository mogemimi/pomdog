// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_ANIMATIONTRACK_70C41AD9_HPP
#define POMDOG_ANIMATIONTRACK_70C41AD9_HPP

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

#endif // POMDOG_ANIMATIONTRACK_70C41AD9_HPP
