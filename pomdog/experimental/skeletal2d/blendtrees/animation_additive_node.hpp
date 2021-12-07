// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_node.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Skeletal2D::Detail {

class AnimationAdditiveNode final : public AnimationNode {
public:
    AnimationAdditiveNode(
        std::unique_ptr<AnimationNode>&& blendNode1In,
        std::unique_ptr<AnimationNode>&& blendNode2In);

    void Calculate(
        const AnimationTimeInterval& time,
        const AnimationGraphWeightCollection& weights,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin) const override;

    float GetWeight() const noexcept;
    void SetWeight(float amount) noexcept;

    AnimationTimeInterval GetLength() const override;

    [[nodiscard]] const AnimationNode* Base() const noexcept { return base.get(); }
    [[nodiscard]] const AnimationNode* Additive() const noexcept { return additive.get(); }

private:
    std::unique_ptr<AnimationNode> base;
    std::unique_ptr<AnimationNode> additive;
    AnimationTimeInterval length;
    float weight;
};
} // namespace Pomdog::Skeletal2D::Detail
