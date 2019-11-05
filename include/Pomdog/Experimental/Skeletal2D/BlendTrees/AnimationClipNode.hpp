// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Skeletal2D/BlendTrees/AnimationNode.hpp"
#include <memory>

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
