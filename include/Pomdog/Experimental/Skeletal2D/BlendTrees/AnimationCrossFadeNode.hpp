// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Skeletal2D/BlendTrees/AnimationNode.hpp"
#include <memory>
#include <string>

namespace Pomdog::Skeletal2D::Detail {

struct SkeletonAnimationState final {
    std::shared_ptr<AnimationNode const> Node;
    std::string Name;
};

class AnimationCrossFadeNode final : public AnimationNode {
public:
    AnimationCrossFadeNode(
        const SkeletonAnimationState& currentAnimation,
        const SkeletonAnimationState& nextAnimation,
        const AnimationTimeInterval& transitionDuration,
        const AnimationTimeInterval& currentAnimationStartTime);

    void Calculate(
        const AnimationTimeInterval& time,
        const Detail::AnimationGraphWeightCollection& weights,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin) const override;

    AnimationTimeInterval GetLength() const override;

private:
    SkeletonAnimationState currentAnimation;
    SkeletonAnimationState nextAnimation;
    AnimationTimeInterval transitionDuration;
    AnimationTimeInterval currentAnimationStartTime;
};

} // namespace Pomdog::Skeletal2D::Detail
