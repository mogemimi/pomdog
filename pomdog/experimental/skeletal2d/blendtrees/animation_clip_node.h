// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_node.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {
class AnimationClip;
} // namespace pomdog::skeletal2d

namespace pomdog::skeletal2d::detail {

class AnimationClipNode final : public AnimationNode {
public:
    explicit AnimationClipNode(const std::shared_ptr<AnimationClip>& animationClip);

    void Calculate(
        const AnimationTimeInterval& time,
        const detail::AnimationGraphWeightCollection& weights,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin) const override;

    AnimationTimeInterval GetLength() const override;

private:
    std::shared_ptr<AnimationClip> clip;
};

} // namespace pomdog::skeletal2d::detail
