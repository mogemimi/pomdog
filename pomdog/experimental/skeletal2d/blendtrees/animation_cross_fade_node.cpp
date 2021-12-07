// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/blendtrees/animation_cross_fade_node.hpp"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_graph_weight_collection.hpp"
#include "pomdog/experimental/skeletal2d/blendtrees/weight_blending_helper.hpp"
#include "pomdog/experimental/skeletal2d/skeleton.hpp"
#include "pomdog/experimental/skeletal2d/skeleton_helper.hpp"
#include "pomdog/experimental/skeletal2d/skeleton_pose.hpp"
#include "pomdog/utility/assert.hpp"

namespace Pomdog::Skeletal2D::Detail {
namespace {

AnimationTimeInterval
WrapTime(const AnimationTimeInterval& source, const AnimationTimeInterval& max)
{
    auto time = source;
    while (time > max) {
        time -= max;
    }

    POMDOG_ASSERT(time >= AnimationTimeInterval::zero());
    POMDOG_ASSERT(time <= max);

    return time;
}

} // namespace

AnimationCrossFadeNode::AnimationCrossFadeNode(
    const SkeletonAnimationState& currentAnimationIn,
    const SkeletonAnimationState& nextAnimationIn,
    const AnimationTimeInterval& transitionDurationIn,
    const AnimationTimeInterval& currentAnimationStartTimeIn)
    : currentAnimation(currentAnimationIn)
    , nextAnimation(nextAnimationIn)
    , transitionDuration(transitionDurationIn)
    , currentAnimationStartTime(currentAnimationStartTimeIn)
{
}

void AnimationCrossFadeNode::Calculate(
    const AnimationTimeInterval& time,
    const Detail::AnimationGraphWeightCollection& weights,
    const Skeleton& skeleton,
    SkeletonPose& skeletonPose,
    Skin* skin) const
{
    auto sourcePose1 = SkeletonPose::CreateBindPose(skeleton);
    auto sourcePose2 = SkeletonPose::CreateBindPose(skeleton);

    POMDOG_ASSERT(transitionDuration.count() > 0);
    const float weight = (time / transitionDuration.count()).count();
    POMDOG_ASSERT(weight >= 0.0f);
    POMDOG_ASSERT(weight <= 1.0f);

    Skin* skin1 = skin;
    Skin* skin2 = nullptr;
    if (weight >= 0.5f) {
        std::swap(skin1, skin2);
    }

    {
        auto sourceTime = WrapTime(currentAnimationStartTime + time, currentAnimation.Node->GetLength());

        POMDOG_ASSERT(sourceTime >= AnimationTimeInterval::zero());
        POMDOG_ASSERT(sourceTime <= currentAnimation.Node->GetLength());

        POMDOG_ASSERT(currentAnimation.Node);
        currentAnimation.Node->Calculate(sourceTime, weights, skeleton, sourcePose1, skin1);
    }
    {
        auto sourceTime = WrapTime(time, nextAnimation.Node->GetLength());

        POMDOG_ASSERT(sourceTime >= AnimationTimeInterval::zero());
        POMDOG_ASSERT(sourceTime <= nextAnimation.Node->GetLength());

        POMDOG_ASSERT(nextAnimation.Node);
        nextAnimation.Node->Calculate(sourceTime, weights, skeleton, sourcePose2, skin2);
    }

    using Detail::WeightBlendingHelper::Lerp;
    Lerp(sourcePose1.JointPoses, sourcePose2.JointPoses, weight, skeletonPose.JointPoses);
}

AnimationTimeInterval AnimationCrossFadeNode::GetLength() const
{
    return transitionDuration;
}

} // namespace Pomdog::Skeletal2D::Detail
