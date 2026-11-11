// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/blendtrees/animation_cross_fade_node.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_graph_weight_collection.h"
#include "pomdog/experimental/skeletal2d/blendtrees/weight_blending_helper.h"
#include "pomdog/experimental/skeletal2d/skeleton.h"
#include "pomdog/experimental/skeletal2d/skeleton_helper.h"
#include "pomdog/experimental/skeletal2d/skeleton_pose.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d::detail {
namespace {

AnimationTimeInterval
wrapTime(const AnimationTimeInterval& source, const AnimationTimeInterval& max)
{
    // NOTE: A state whose clips have no keys has a length of zero, so every
    // time of it is the beginning. Subtracting the length would never end.
    if (max <= AnimationTimeInterval::zero()) {
        return AnimationTimeInterval::zero();
    }

    // NOTE: std::fmod() takes constant time, and also ends when subtracting
    // a short length from a long time no longer changes the time.
    auto time = source;
    if (time > max) {
        time = AnimationTimeInterval{std::fmod(time.count(), max.count())};
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
    : currentAnimation_(currentAnimationIn)
    , nextAnimation_(nextAnimationIn)
    , transitionDuration_(transitionDurationIn)
    , currentAnimationStartTime_(currentAnimationStartTimeIn)
{
}

void AnimationCrossFadeNode::calculate(
    const AnimationTimeInterval& time,
    const detail::AnimationGraphWeightCollection& weights,
    const Skeleton& skeleton,
    SkeletonPose& skeletonPose,
    Skin* skin) const
{
    auto sourcePose1 = SkeletonPose::createBindPose(skeleton);
    auto sourcePose2 = SkeletonPose::createBindPose(skeleton);

    POMDOG_ASSERT(transitionDuration_.count() > 0);
    const f32 weight = (time / transitionDuration_.count()).count();
    POMDOG_ASSERT(weight >= 0.0f);
    POMDOG_ASSERT(weight <= 1.0f);

    Skin* skin1 = skin;
    Skin* skin2 = nullptr;
    if (weight >= 0.5f) {
        std::swap(skin1, skin2);
    }

    {
        auto sourceTime = wrapTime(currentAnimationStartTime_ + time, currentAnimation_.node->getLength());

        POMDOG_ASSERT(sourceTime >= AnimationTimeInterval::zero());
        POMDOG_ASSERT(sourceTime <= currentAnimation_.node->getLength());

        POMDOG_ASSERT(currentAnimation_.node);
        currentAnimation_.node->calculate(sourceTime, weights, skeleton, sourcePose1, skin1);
    }
    {
        auto sourceTime = wrapTime(time, nextAnimation_.node->getLength());

        POMDOG_ASSERT(sourceTime >= AnimationTimeInterval::zero());
        POMDOG_ASSERT(sourceTime <= nextAnimation_.node->getLength());

        POMDOG_ASSERT(nextAnimation_.node);
        nextAnimation_.node->calculate(sourceTime, weights, skeleton, sourcePose2, skin2);
    }

    using detail::WeightBlendingHelper::lerp;
    lerp(sourcePose1.jointPoses, sourcePose2.jointPoses, weight, skeletonPose.jointPoses);
}

AnimationTimeInterval AnimationCrossFadeNode::getLength() const
{
    return transitionDuration_;
}

} // namespace pomdog::skeletal2d::detail
