// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/blendtrees/animation_clip_node.hpp"
#include "pomdog/experimental/skeletal2d/animation_clip.hpp"
#include "pomdog/utility/assert.hpp"

namespace Pomdog::Skeletal2D::Detail {

AnimationClipNode::AnimationClipNode(const std::shared_ptr<AnimationClip>& animationClipIn)
    : clip(animationClipIn)
{
}

AnimationTimeInterval AnimationClipNode::GetLength() const
{
    POMDOG_ASSERT(clip);
    return clip->GetLength();
}

void AnimationClipNode::Calculate(
    const AnimationTimeInterval& time,
    const Detail::AnimationGraphWeightCollection&,
    const Skeleton& skeleton,
    SkeletonPose& skeletonPose,
    Skin* skin) const
{
    POMDOG_ASSERT(clip);
    clip->Apply(time, skeleton, skeletonPose, skin);
}

} // namespace Pomdog::Skeletal2D::Detail
