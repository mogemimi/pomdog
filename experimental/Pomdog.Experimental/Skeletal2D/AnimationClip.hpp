// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ANIMATIONCLIP_5D3DFD31_HPP
#define POMDOG_ANIMATIONCLIP_5D3DFD31_HPP

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

#endif // POMDOG_ANIMATIONCLIP_5D3DFD31_HPP
