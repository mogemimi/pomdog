// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "AnimationTimeInterval.hpp"

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

class AnimationGraphWeightCollection;

} // namespace Skeletal2D
} // namespace Detail

class Skeleton;
class SkeletonPose;

class AnimationNode {
public:
    virtual ~AnimationNode() = default;

    virtual void Calculate(AnimationTimeInterval const& time,
        Detail::Skeletal2D::AnimationGraphWeightCollection const& weights,
        Skeleton const& skeleton,
        SkeletonPose & skeletonPose) const = 0;

    virtual AnimationTimeInterval Length() const = 0;
};

} // namespace Pomdog
