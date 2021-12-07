// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/experimental/skeletal2d/animation_time_interval.hpp"

namespace Pomdog::Skeletal2D::Detail {
class AnimationGraphWeightCollection;
} // namespace Pomdog::Skeletal2D::Detail

namespace Pomdog::Skeletal2D {

class Skeleton;
class SkeletonPose;
class Skin;

class AnimationNode {
public:
    virtual ~AnimationNode();

    virtual void Calculate(
        const AnimationTimeInterval& time,
        const Detail::AnimationGraphWeightCollection& weights,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin) const = 0;

    virtual AnimationTimeInterval GetLength() const = 0;
};

} // namespace Pomdog::Skeletal2D
