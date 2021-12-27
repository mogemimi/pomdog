// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
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
        std::uint16_t weightIndex);

    void Calculate(
        const AnimationTimeInterval& time,
        const detail::AnimationGraphWeightCollection& weights,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin) const override;

    AnimationTimeInterval GetLength() const override;

    [[nodiscard]] const AnimationNode* A() const noexcept;
    [[nodiscard]] const AnimationNode* B() const noexcept;

private:
    std::unique_ptr<AnimationNode> nodeA;
    std::unique_ptr<AnimationNode> nodeB;
    AnimationTimeInterval length;
    std::uint16_t weightIndex;
};

} // namespace pomdog::skeletal2d::detail
