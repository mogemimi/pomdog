// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Skeletal2D/BlendTrees/AnimationLerpNode.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/Skeletal2D/BlendTrees/AnimationGraphWeightCollection.hpp"
#include "Pomdog/Experimental/Skeletal2D/BlendTrees/WeightBlendingHelper.hpp"
#include "Pomdog/Experimental/Skeletal2D/SkeletonPose.hpp"
#include "Pomdog/Math/Math.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Utility/Assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Skeletal2D::Detail {

AnimationLerpNode::AnimationLerpNode(
    std::unique_ptr<AnimationNode>&& blendNode1In,
    std::unique_ptr<AnimationNode>&& blendNode2In,
    std::uint16_t weightIndexIn)
    : nodeA(std::move(blendNode1In))
    , nodeB(std::move(blendNode2In))
    , weightIndex(weightIndexIn)
{
    POMDOG_ASSERT(nodeA);
    POMDOG_ASSERT(nodeB);
    length = std::max(nodeA->GetLength(), nodeB->GetLength());
}

AnimationTimeInterval AnimationLerpNode::GetLength() const
{
    return length;
}

const AnimationNode* AnimationLerpNode::A() const noexcept
{
    return nodeA.get();
}

const AnimationNode* AnimationLerpNode::B() const noexcept
{
    return nodeB.get();
}

void AnimationLerpNode::Calculate(
    const AnimationTimeInterval& time,
    const Detail::AnimationGraphWeightCollection& weights,
    const Skeleton& skeleton,
    SkeletonPose& skeletonPose,
    Skin* skin) const
{
    auto sourcePose1 = SkeletonPose::CreateBindPose(skeleton);
    auto sourcePose2 = SkeletonPose::CreateBindPose(skeleton);

    POMDOG_ASSERT(nodeA);
    POMDOG_ASSERT(nodeB);

    nodeA->Calculate(time, weights, skeleton, sourcePose1, skin);
    nodeB->Calculate(time, weights, skeleton, sourcePose2, nullptr);

    auto weight = weights.GetFloatAtIndex(weightIndex);
    using Detail::WeightBlendingHelper::Lerp;
    Lerp(sourcePose1.JointPoses, sourcePose2.JointPoses, weight, skeletonPose.JointPoses);
}

} // namespace Pomdog::Skeletal2D::Detail
