// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/blendtrees/animation_lerp_node.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_graph_weight_collection.h"
#include "pomdog/experimental/skeletal2d/blendtrees/weight_blending_helper.h"
#include "pomdog/experimental/skeletal2d/skeleton_pose.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/math/vector2.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d::detail {

AnimationLerpNode::AnimationLerpNode(
    std::unique_ptr<AnimationNode>&& blendNode1In,
    std::unique_ptr<AnimationNode>&& blendNode2In,
    u16 weightIndexIn)
    : nodeA_(std::move(blendNode1In))
    , nodeB_(std::move(blendNode2In))
    , weightIndex_(weightIndexIn)
{
    POMDOG_ASSERT(nodeA_);
    POMDOG_ASSERT(nodeB_);
    length_ = std::max(nodeA_->getLength(), nodeB_->getLength());
}

AnimationTimeInterval AnimationLerpNode::getLength() const
{
    return length_;
}

const AnimationNode* AnimationLerpNode::a() const noexcept
{
    return nodeA_.get();
}

const AnimationNode* AnimationLerpNode::b() const noexcept
{
    return nodeB_.get();
}

void AnimationLerpNode::calculate(
    const AnimationTimeInterval& time,
    const detail::AnimationGraphWeightCollection& weights,
    const Skeleton& skeleton,
    SkeletonPose& skeletonPose,
    Skin* skin) const
{
    auto sourcePose1 = SkeletonPose::createBindPose(skeleton);
    auto sourcePose2 = SkeletonPose::createBindPose(skeleton);

    POMDOG_ASSERT(nodeA_);
    POMDOG_ASSERT(nodeB_);

    nodeA_->calculate(time, weights, skeleton, sourcePose1, skin);
    nodeB_->calculate(time, weights, skeleton, sourcePose2, nullptr);

    auto weight = weights.getFloatAtIndex(weightIndex_);
    using detail::WeightBlendingHelper::lerp;
    lerp(sourcePose1.jointPoses, sourcePose2.jointPoses, weight, skeletonPose.jointPoses);
}

} // namespace pomdog::skeletal2d::detail
