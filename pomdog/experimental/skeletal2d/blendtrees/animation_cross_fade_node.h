// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_node.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d::detail {

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
        const detail::AnimationGraphWeightCollection& weights,
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

} // namespace pomdog::skeletal2d::detail
