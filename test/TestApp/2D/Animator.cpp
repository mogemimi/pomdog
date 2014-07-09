//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "Animator.hpp"
#include "../Skeletal2D/AnimationTrack.hpp"
#include "../Skeletal2D/AnimationNode.hpp"
#include "../Skeletal2D/AnimationAdditiveNode.hpp"
#include "../Skeletal2D/AnimationLerpNode.hpp"
#include "../Skeletal2D/AnimationClipNode.hpp"
#include "../Skeletal2D/SkeletonHelper.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
MaidAnimator::MaidAnimator(std::shared_ptr<Skeleton> const& skeletonIn,
	std::shared_ptr<SkeletonTransform> const& skeletonTransformIn,
	std::shared_ptr<AnimationGraph> const& animationGraphIn)
	: skeleton(skeletonIn)
	, skeletonTransform(skeletonTransformIn)
	, animationGraph(animationGraphIn)
	, time(AnimationTimeInterval::zero())
	, playbackRate(1.0f)
{
	POMDOG_ASSERT(skeleton);
	POMDOG_ASSERT(skeletonTransform);
	POMDOG_ASSERT(animationGraph);
	
	for (auto & parameter: animationGraph->Inputs)
	{
		switch (parameter.Type) {
		case AnimationBlendInputType::Float:
			graphWeights.AddFloat();
			break;
		case AnimationBlendInputType::Bool:
			graphWeights.AddBool();
			break;
		}
	}
}
//-----------------------------------------------------------------------
void MaidAnimator::Update(GameClock const& clock)
{
	// (1) Update time:
	{
		AnimationTimeInterval frameDuration = clock.FrameDuration();
		time = time + frameDuration * playbackRate;
		
		POMDOG_ASSERT(animationGraph);
		POMDOG_ASSERT(animationGraph->Tree);
		if (time < AnimationTimeInterval::zero()) {
			time = animationGraph->Tree->Length();
		}
		else if (time > animationGraph->Tree->Length()) {
			//POMDOG_ASSERT(loop);
			time = AnimationTimeInterval::zero();
		}
	}

	// (2) Pose extraction, and (3) Pose blending:
	POMDOG_ASSERT(animationGraph);
	POMDOG_ASSERT(animationGraph->Tree);
	animationGraph->Tree->Calculate(time, graphWeights, *skeleton, skeletonTransform->Pose);

	// (4) Global pose generation:
	SkeletonHelper::ToGlobalPose(*skeleton, skeletonTransform->Pose, skeletonTransform->GlobalPose);
}
//-----------------------------------------------------------------------
float MaidAnimator::PlaybackRate() const
{
	return playbackRate;
}
//-----------------------------------------------------------------------
void MaidAnimator::PlaybackRate(float playbackRateIn)
{
	this->playbackRate = playbackRateIn;
}
//-----------------------------------------------------------------------
void MaidAnimator::SetFloat(std::string const& name, float value)
{
	POMDOG_ASSERT(animationGraph);
	if (auto index = animationGraph->FindParameter(name)) {
		graphWeights.SetValue(*index, value);
	}
}
//-----------------------------------------------------------------------
void MaidAnimator::SetBool(std::string const& name, bool value)
{
	POMDOG_ASSERT(animationGraph);
	if (auto index = animationGraph->FindParameter(name)) {
		graphWeights.SetValue(*index, value);
	}
}
//-----------------------------------------------------------------------
}// namespace Pomdog
