// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Skeletal2D/BlendTrees/AnimationClipNode.hpp"
#include "Pomdog/Experimental/Skeletal2D/AnimationClip.hpp"
#include "Pomdog/Utility/Assert.hpp"

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
