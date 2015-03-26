// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ANIMATIONADDITIVENODE_48896792_HPP
#define POMDOG_ANIMATIONADDITIVENODE_48896792_HPP

#include "Pomdog.Experimental/Skeletal2D/AnimationNode.hpp"
#include <memory>

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

class AnimationAdditiveNode final: public AnimationNode {
public:
    AnimationAdditiveNode(std::unique_ptr<AnimationNode> && blendNode1In,
        std::unique_ptr<AnimationNode> && blendNode2In);

    void Calculate(AnimationTimeInterval const& time,
        Detail::Skeletal2D::AnimationGraphWeightCollection const& weights,
        Skeleton const& skeleton,
        SkeletonPose & skeletonPose) const override;

    float Weight() const;
    void Weight(float amount);

    AnimationTimeInterval Length() const override;

    std::unique_ptr<AnimationNode> const& Base() const { return base; }
    std::unique_ptr<AnimationNode> const& Additive() const { return additive; }

private:
    std::unique_ptr<AnimationNode> base;
    std::unique_ptr<AnimationNode> additive;
    AnimationTimeInterval length;
    float weight;
};

}// namespace Skeletal2D
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_ANIMATIONADDITIVENODE_48896792_HPP
