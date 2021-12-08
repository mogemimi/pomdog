// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/blendtrees/animation_lerp_node.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_graph_weight_collection.hpp"
#include "pomdog/experimental/skeletal2d/blendtrees/weight_blending_helper.hpp"
#include "pomdog/experimental/skeletal2d/skeleton_pose.hpp"
#include "pomdog/math/math.hpp"
#include "pomdog/math/vector2.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d::detail {

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
    const detail::AnimationGraphWeightCollection& weights,
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
    using detail::WeightBlendingHelper::Lerp;
    Lerp(sourcePose1.JointPoses, sourcePose2.JointPoses, weight, skeletonPose.JointPoses);
}

} // namespace pomdog::skeletal2d::detail
