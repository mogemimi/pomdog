//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "AnimationClipNode.hpp"
#include <Pomdog/Utility/Assert.hpp>
#include "AnimationClip.hpp"


namespace Pomdog {

AnimationClipNode::AnimationClipNode(std::shared_ptr<AnimationClip> const& animationClipIn)
	: clip(animationClipIn)
{
}
//-----------------------------------------------------------------------
void AnimationClipNode::Calculate(AnimationTimeInterval const& time, Skeleton const& skeleton, SkeletonPose & skeletonPose)
{
	POMDOG_ASSERT(clip);
	clip->Apply(time, skeleton, skeletonPose);
}

}// namespace Pomdog
