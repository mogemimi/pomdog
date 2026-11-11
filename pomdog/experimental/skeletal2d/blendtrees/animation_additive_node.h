// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_node.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d::detail {

class AnimationAdditiveNode final : public AnimationNode {
public:
    AnimationAdditiveNode(
        std::unique_ptr<AnimationNode>&& blendNode1In,
        std::unique_ptr<AnimationNode>&& blendNode2In);

    void calculate(
        const AnimationTimeInterval& time,
        const AnimationGraphWeightCollection& weights,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin) const override;

    f32 getWeight() const noexcept;
    void setWeight(f32 amount) noexcept;

    AnimationTimeInterval getLength() const override;

    [[nodiscard]] const AnimationNode* base() const noexcept { return base_.get(); }
    [[nodiscard]] const AnimationNode* additive() const noexcept { return additive_.get(); }

private:
    std::unique_ptr<AnimationNode> base_;
    std::unique_ptr<AnimationNode> additive_;
    AnimationTimeInterval length_;
    f32 weight_;
};
} // namespace pomdog::skeletal2d::detail
