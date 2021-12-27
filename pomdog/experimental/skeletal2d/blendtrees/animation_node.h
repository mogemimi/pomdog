// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/experimental/skeletal2d/animation_time_interval.h"

namespace pomdog::skeletal2d::detail {
class AnimationGraphWeightCollection;
} // namespace pomdog::skeletal2d::detail

namespace pomdog::skeletal2d {

class Skeleton;
class SkeletonPose;
class Skin;

class AnimationNode {
public:
    virtual ~AnimationNode();

    virtual void Calculate(
        const AnimationTimeInterval& time,
        const detail::AnimationGraphWeightCollection& weights,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin) const = 0;

    virtual AnimationTimeInterval GetLength() const = 0;
};

} // namespace pomdog::skeletal2d
