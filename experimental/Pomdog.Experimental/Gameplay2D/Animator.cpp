//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "Animator.hpp"
#include "Pomdog.Experimental/Skeletal2D/AnimationTrack.hpp"
#include "Pomdog.Experimental/Skeletal2D/AnimationNode.hpp"
#include "Pomdog.Experimental/Skeletal2D/AnimationAdditiveNode.hpp"
#include "Pomdog.Experimental/Skeletal2D/AnimationLerpNode.hpp"
#include "Pomdog.Experimental/Skeletal2D/AnimationClipNode.hpp"
#include "Pomdog.Experimental/Skeletal2D/SkeletonHelper.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
SkeletonAnimator::SkeletonAnimator(std::shared_ptr<Skeleton> const& skeletonIn,
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
	
	graphWeights.Reserve(animationGraph->Inputs.size());
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
	
	POMDOG_ASSERT(animationGraph);
	POMDOG_ASSERT(!animationGraph->States.empty());
	POMDOG_ASSERT(animationGraph->States.front().Tree);
	
	animationTree = std::shared_ptr<AnimationNode>(animationGraph,
		animationGraph->States.front().Tree.get());
}
//-----------------------------------------------------------------------
void SkeletonAnimator::Update(DurationSeconds const& frameDuration)
{
	// (1) Update time:
	{
		time = time + frameDuration * playbackRate;

		POMDOG_ASSERT(animationTree);
		if (time < AnimationTimeInterval::zero()) {
			time = animationTree->Length();
		}
		else if (time > animationTree->Length()) {
			//POMDOG_ASSERT(loop);
			time = AnimationTimeInterval::zero();
		}
	}

	// (2) Pose extraction, and (3) Pose blending:
	POMDOG_ASSERT(animationTree);
	animationTree->Calculate(time, graphWeights, *skeleton, skeletonTransform->Pose);

	// (4) Global pose generation:
	SkeletonHelper::ToGlobalPose(*skeleton, skeletonTransform->Pose, skeletonTransform->GlobalPose);
}
//-----------------------------------------------------------------------
void SkeletonAnimator::Play(std::string const& stateName)
{
	POMDOG_ASSERT(animationGraph);
	POMDOG_ASSERT(!animationGraph->States.empty());

	auto iter = std::find_if(std::begin(animationGraph->States), std::end(animationGraph->States), [&stateName](AnimationGraphState const& state){
		return state.Name == stateName;
	});
	
	if (iter == std::end(animationGraph->States)) {
		return;
	}
	
	POMDOG_ASSERT(iter->Tree);
	animationTree = std::shared_ptr<AnimationNode>(animationGraph, iter->Tree.get());
	time = AnimationTimeInterval::zero();
}
//-----------------------------------------------------------------------
float SkeletonAnimator::PlaybackRate() const
{
	return playbackRate;
}
//-----------------------------------------------------------------------
void SkeletonAnimator::PlaybackRate(float playbackRateIn)
{
	this->playbackRate = playbackRateIn;
}
//-----------------------------------------------------------------------
void SkeletonAnimator::SetFloat(std::string const& name, float value)
{
	POMDOG_ASSERT(!std::isnan(value));
	POMDOG_ASSERT(animationGraph);
	if (auto index = animationGraph->FindParameter(name)) {
		graphWeights.SetValue(*index, value);
	}
}
//-----------------------------------------------------------------------
void SkeletonAnimator::SetBool(std::string const& name, bool value)
{
	POMDOG_ASSERT(animationGraph);
	if (auto index = animationGraph->FindParameter(name)) {
		graphWeights.SetValue(*index, value);
	}
}
//-----------------------------------------------------------------------
}// namespace Pomdog
