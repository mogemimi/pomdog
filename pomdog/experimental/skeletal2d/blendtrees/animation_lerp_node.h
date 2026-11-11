// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_node.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d::detail {

class AnimationLerpNode final : public AnimationNode {
public:
    AnimationLerpNode(
        std::unique_ptr<AnimationNode>&& blendNode1,
        std::unique_ptr<AnimationNode>&& blendNode2,
        u16 weightIndex);

    void calculate(
        const AnimationTimeInterval& time,
        const detail::AnimationGraphWeightCollection& weights,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin) const override;

    AnimationTimeInterval getLength() const override;

    [[nodiscard]] const AnimationNode* a() const noexcept;
    [[nodiscard]] const AnimationNode* b() const noexcept;

private:
    std::unique_ptr<AnimationNode> nodeA_;
    std::unique_ptr<AnimationNode> nodeB_;
    AnimationTimeInterval length_;
    u16 weightIndex_;
};

} // namespace pomdog::skeletal2d::detail
