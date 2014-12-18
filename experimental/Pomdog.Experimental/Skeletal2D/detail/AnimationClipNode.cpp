//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "AnimationClipNode.hpp"
#include "Pomdog.Experimental/Skeletal2D/AnimationClip.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Details {
namespace Skeletal2D {

AnimationClipNode::AnimationClipNode(std::shared_ptr<AnimationClip> const& animationClipIn)
	: clip(animationClipIn)
{}
//-----------------------------------------------------------------------
AnimationTimeInterval AnimationClipNode::Length() const
{
	POMDOG_ASSERT(clip);
	return clip->Length();
}
//-----------------------------------------------------------------------
void AnimationClipNode::Calculate(AnimationTimeInterval const& time,
	Details::Skeletal2D::AnimationGraphWeightCollection const&,
	Skeleton const& skeleton, SkeletonPose & skeletonPose) const
{
	POMDOG_ASSERT(clip);
	clip->Apply(time, skeleton, skeletonPose);
}

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog
