// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "AnimationClipNode.hpp"
#include "Pomdog.Experimental/Skeletal2D/AnimationClip.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

AnimationClipNode::AnimationClipNode(std::shared_ptr<AnimationClip> const& animationClipIn)
    : clip(animationClipIn)
{}

AnimationTimeInterval AnimationClipNode::Length() const
{
    POMDOG_ASSERT(clip);
    return clip->Length();
}

void AnimationClipNode::Calculate(AnimationTimeInterval const& time,
    Detail::Skeletal2D::AnimationGraphWeightCollection const&,
    Skeleton const& skeleton, SkeletonPose & skeletonPose) const
{
    POMDOG_ASSERT(clip);
    clip->Apply(time, skeleton, skeletonPose);
}

} // namespace Skeletal2D
} // namespace Detail
} // namespace Pomdog
