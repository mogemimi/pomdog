// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "AnimationLerpNode.hpp"
#include "Pomdog.Experimental/Skeletal2D/detail/AnimationGraphWeightCollection.hpp"
#include "Pomdog.Experimental/Skeletal2D/detail/WeightBlendingHelper.hpp"
#include "Pomdog.Experimental/Skeletal2D/SkeletonPose.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include <algorithm>

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {
//-----------------------------------------------------------------------
AnimationLerpNode::AnimationLerpNode(std::unique_ptr<AnimationNode> && blendNode1In,
    std::unique_ptr<AnimationNode> && blendNode2In, std::uint16_t weightIndexIn)
    : nodeA(std::move(blendNode1In))
    , nodeB(std::move(blendNode2In))
    , weightIndex(weightIndexIn)
{
    POMDOG_ASSERT(nodeA);
    POMDOG_ASSERT(nodeB);
    length = std::max(nodeA->Length(), nodeB->Length());
}
//-----------------------------------------------------------------------
AnimationTimeInterval AnimationLerpNode::Length() const
{
    return length;
}
//-----------------------------------------------------------------------
std::unique_ptr<AnimationNode> const& AnimationLerpNode::A() const
{
    return nodeA;
}
//-----------------------------------------------------------------------
std::unique_ptr<AnimationNode> const& AnimationLerpNode::B() const
{
    return nodeB;
}
//-----------------------------------------------------------------------
void AnimationLerpNode::Calculate(AnimationTimeInterval const& time,
    Detail::Skeletal2D::AnimationGraphWeightCollection const& weights,
    Skeleton const& skeleton, SkeletonPose & skeletonPose) const
{
    auto sourcePose1 = SkeletonPose::CreateBindPose(skeleton);
    auto sourcePose2 = SkeletonPose::CreateBindPose(skeleton);

    POMDOG_ASSERT(nodeA);
    POMDOG_ASSERT(nodeB);

    nodeA->Calculate(time, weights, skeleton, sourcePose1);
    nodeB->Calculate(time, weights, skeleton, sourcePose2);

    auto weight = weights.At(weightIndex).GetFloat();
    using Detail::Skeletal2D::WeightBlendingHelper;
    WeightBlendingHelper::Lerp(sourcePose1.JointPoses, sourcePose2.JointPoses, weight, skeletonPose.JointPoses);
}

}// namespace Skeletal2D
}// namespace Detail
}// namespace Pomdog
