// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/blendtrees/animation_clip_node.h"
#include "pomdog/experimental/skeletal2d/animation_clip.h"
#include "pomdog/utility/assert.h"

namespace pomdog::skeletal2d::detail {

AnimationClipNode::AnimationClipNode(const std::shared_ptr<AnimationClip>& animationClipIn)
    : clip_(animationClipIn)
{
}

AnimationTimeInterval AnimationClipNode::getLength() const
{
    POMDOG_ASSERT(clip_);
    return clip_->getLength();
}

void AnimationClipNode::calculate(
    const AnimationTimeInterval& time,
    const detail::AnimationGraphWeightCollection&,
    const Skeleton& skeleton,
    SkeletonPose& skeletonPose,
    Skin* skin) const
{
    POMDOG_ASSERT(clip_);
    clip_->apply(time, skeleton, skeletonPose, skin);
}

} // namespace pomdog::skeletal2d::detail
