// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/experimental/skeletal2d/animation_time_interval.hpp"

namespace Pomdog::Skeletal2D {

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

} // namespace Pomdog::Skeletal2D
