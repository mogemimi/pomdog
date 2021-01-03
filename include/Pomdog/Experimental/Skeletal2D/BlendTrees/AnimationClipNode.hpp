// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/Skeletal2D/BlendTrees/AnimationNode.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Skeletal2D {
class AnimationClip;
} // namespace Pomdog::Skeletal2D

namespace Pomdog::Skeletal2D::Detail {

class AnimationClipNode final : public AnimationNode {
public:
    explicit AnimationClipNode(const std::shared_ptr<AnimationClip>& animationClip);

    void Calculate(
        const AnimationTimeInterval& time,
        const Detail::AnimationGraphWeightCollection& weights,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin) const override;

    AnimationTimeInterval GetLength() const override;

private:
    std::shared_ptr<AnimationClip> clip;
};

} // namespace Pomdog::Skeletal2D::Detail
