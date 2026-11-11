// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_node.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d::detail {

struct SkeletonAnimationState final {
    std::shared_ptr<AnimationNode const> node;

    /// key is the xxHash32 hash of the state name.
    u32 key = 0;
};

class AnimationCrossFadeNode final : public AnimationNode {
public:
    AnimationCrossFadeNode(
        const SkeletonAnimationState& currentAnimation,
        const SkeletonAnimationState& nextAnimation,
        const AnimationTimeInterval& transitionDuration,
        const AnimationTimeInterval& currentAnimationStartTime);

    void calculate(
        const AnimationTimeInterval& time,
        const detail::AnimationGraphWeightCollection& weights,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin) const override;

    AnimationTimeInterval getLength() const override;

private:
    SkeletonAnimationState currentAnimation_;
    SkeletonAnimationState nextAnimation_;
    AnimationTimeInterval transitionDuration_;
    AnimationTimeInterval currentAnimationStartTime_;
};

} // namespace pomdog::skeletal2d::detail
