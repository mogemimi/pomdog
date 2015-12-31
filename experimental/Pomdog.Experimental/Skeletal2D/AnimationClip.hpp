// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "AnimationTimeInterval.hpp"
#include "AnimationTrack.hpp"
#include <vector>
#include <memory>

namespace Pomdog {

class Skeleton;
class SkeletonPose;

class AnimationClip {
public:
    AnimationClip() = default;

    explicit AnimationClip(std::vector<std::unique_ptr<AnimationTrack>> && tracks);

    void Apply(AnimationTimeInterval const& time, Skeleton const& skeleton, SkeletonPose & skeletonPose);

    AnimationTimeInterval Length() const;

private:
    std::vector<std::unique_ptr<AnimationTrack>> tracks;
    AnimationTimeInterval length;
};

} // namespace Pomdog
