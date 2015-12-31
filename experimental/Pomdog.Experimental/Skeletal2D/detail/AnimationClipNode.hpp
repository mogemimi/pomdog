// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Skeletal2D/AnimationNode.hpp"
#include <memory>

namespace Pomdog {

class AnimationClip;

namespace Detail {
namespace Skeletal2D {

class AnimationClipNode final: public AnimationNode {
public:
    AnimationClipNode(std::shared_ptr<AnimationClip> const& animationClip);

    void Calculate(AnimationTimeInterval const& time,
        Detail::Skeletal2D::AnimationGraphWeightCollection const& weights,
        Skeleton const& skeleton,
        SkeletonPose & skeletonPose) const override;

    AnimationTimeInterval Length() const override;

private:
    std::shared_ptr<AnimationClip> clip;
};

} // namespace Skeletal2D
} // namespace Detail
} // namespace Pomdog
