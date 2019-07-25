// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Skeletal2D/AnimationNode.hpp"
#include <memory>

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

class AnimationLerpNode final: public AnimationNode {
public:
    AnimationLerpNode(std::unique_ptr<AnimationNode> && blendNode1,
        std::unique_ptr<AnimationNode> && blendNode2, std::uint16_t weightIndex);

    void Calculate(AnimationTimeInterval const& time,
        Detail::Skeletal2D::AnimationGraphWeightCollection const& weights,
        Skeleton const& skeleton,
        SkeletonPose & skeletonPose) const override;

    AnimationTimeInterval Length() const override;

    std::unique_ptr<AnimationNode> const& A() const;
    std::unique_ptr<AnimationNode> const& B() const;

private:
    std::unique_ptr<AnimationNode> nodeA;
    std::unique_ptr<AnimationNode> nodeB;
    AnimationTimeInterval length;
    std::uint16_t weightIndex;
};

} // namespace Skeletal2D
} // namespace Detail
} // namespace Pomdog
