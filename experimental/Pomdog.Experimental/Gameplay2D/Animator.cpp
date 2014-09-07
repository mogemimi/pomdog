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
namespace {

static auto FindState(std::vector<AnimationGraphState> const& states, std::string const& stateName)
	->decltype(states.begin())
{
	return std::find_if(std::begin(states), std::end(states),
		[&stateName](AnimationGraphState const& state) {
			return state.Name == stateName;
		});
}

namespace {

void Lerp(std::vector<JointPose> const& sourcePoses1, std::vector<JointPose> const& sourcePoses2,
	float weight, std::vector<JointPose> & output)
{
	POMDOG_ASSERT(!sourcePoses1.empty());
	POMDOG_ASSERT(!sourcePoses2.empty());
	POMDOG_ASSERT(sourcePoses1.size() == sourcePoses2.size());

	for (size_t i = 0; i < sourcePoses1.size(); ++i)
	{
		auto & pose1 = sourcePoses1[i];
		auto & pose2 = sourcePoses2[i];
		
		POMDOG_ASSERT(!output.empty());
		POMDOG_ASSERT(i < output.size());
		auto & result = output[i];

		result.Scale = MathHelper::Lerp(pose1.Scale, pose2.Scale, weight);
		result.Rotation = MathHelper::Lerp(pose1.Rotation.value, pose2.Rotation.value, weight);
		result.Translate = Vector2::Lerp(pose1.Translate, pose2.Translate, weight);
	}
}

}// unnamed namespace

class AnimationCrossFadeNode final: public AnimationNode {
private:
	SkeletonAnimationState currentAnimation;
	SkeletonAnimationState nextAnimation;
	AnimationTimeInterval transitionDuration;
	AnimationTimeInterval currentAnimationStartTime;

public:
	AnimationCrossFadeNode(
		SkeletonAnimationState const& currentAnimationIn,
		SkeletonAnimationState const& nextAnimationIn,
		AnimationTimeInterval const& transitionDurationIn,
		AnimationTimeInterval const& currentAnimationStartTimeIn)
		: currentAnimation(currentAnimationIn)
		, nextAnimation(nextAnimationIn)
		, transitionDuration(transitionDurationIn)
		, currentAnimationStartTime(currentAnimationStartTimeIn)
	{}

	void Calculate(AnimationTimeInterval const& time,
		AnimationGraphWeightCollection const& weights,
		Skeleton const& skeleton,
		SkeletonPose & skeletonPose) const override
	{
		auto sourcePose1 = SkeletonPose::CreateBindPose(skeleton);
		auto sourcePose2 = SkeletonPose::CreateBindPose(skeleton);

		{
			auto sourceTime = currentAnimationStartTime + time;
			while (sourceTime > currentAnimation.Node->Length()) {
				sourceTime -= currentAnimation.Node->Length();
			}

			POMDOG_ASSERT(sourceTime >= AnimationTimeInterval::zero());
			POMDOG_ASSERT(sourceTime <= currentAnimation.Node->Length());

			POMDOG_ASSERT(currentAnimation.Node);
			currentAnimation.Node->Calculate(sourceTime, weights, skeleton, sourcePose1);
		}
		{
			auto sourceTime = time;
			while (sourceTime > nextAnimation.Node->Length()) {
				sourceTime -= nextAnimation.Node->Length();
			}

			POMDOG_ASSERT(sourceTime >= AnimationTimeInterval::zero());
			POMDOG_ASSERT(sourceTime <= nextAnimation.Node->Length());

			POMDOG_ASSERT(nextAnimation.Node);
			nextAnimation.Node->Calculate(sourceTime, weights, skeleton, sourcePose2);
		}
		
		POMDOG_ASSERT(transitionDuration.count() > 0);
		float weight = (time / transitionDuration.count()).count();
		POMDOG_ASSERT(weight >= 0.0f);
		POMDOG_ASSERT(weight <= 1.0f);
		
		Lerp(sourcePose1.JointPoses, sourcePose2.JointPoses, weight, skeletonPose.JointPoses);
	}
	
	AnimationTimeInterval Length() const
	{
		return transitionDuration;
	}
};

}// unnamed namespace
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
	
	currentAnimation.Name = animationGraph->States.front().Name;
	currentAnimation.Node = std::shared_ptr<AnimationNode>(animationGraph,
		animationGraph->States.front().Tree.get());
}
//-----------------------------------------------------------------------
void SkeletonAnimator::Update(DurationSeconds const& frameDuration)
{
	// (1) Update time:
	{
		time = time + frameDuration * playbackRate;

		POMDOG_ASSERT(currentAnimation.Node);
		if (time < AnimationTimeInterval::zero()) {
			time = currentAnimation.Node->Length();
		}
		else if (time > currentAnimation.Node->Length()) {
			if (nextAnimation.Node) { ///@todo badcode
				// transit
				currentAnimation.Node = nextAnimation.Node;
				currentAnimation.Name = nextAnimation.Name;
				nextAnimation.Node.reset();
				nextAnimation.Name.clear();

				{
					while (time > currentAnimation.Node->Length()) {
						time -= currentAnimation.Node->Length();
					}
				
					POMDOG_ASSERT(time >= AnimationTimeInterval::zero());
					POMDOG_ASSERT(time <= currentAnimation.Node->Length());
				}
			}
			else {
				//POMDOG_ASSERT(loop);
				time = AnimationTimeInterval::zero();
			}
		}
	}

	// (2) Pose extraction, and (3) Pose blending:
	POMDOG_ASSERT(currentAnimation.Node);
	currentAnimation.Node->Calculate(time, graphWeights, *skeleton, skeletonTransform->Pose);

	// (4) Global pose generation:
	SkeletonHelper::ToGlobalPose(*skeleton, skeletonTransform->Pose, skeletonTransform->GlobalPose);
}
//-----------------------------------------------------------------------
void SkeletonAnimator::CrossFade(std::string const& stateName, DurationSeconds const& transitionDuration)
{
	if (nextAnimation.Name == stateName) {
		return;
	}
	if (nextAnimation.Node) {
		return;
	}

	POMDOG_ASSERT(transitionDuration > DurationSeconds::zero());
	POMDOG_ASSERT(!std::isnan(transitionDuration.count()));
	
	auto iter = FindState(animationGraph->States, stateName);
	POMDOG_ASSERT(iter != std::end(animationGraph->States));
	
	if (iter == std::end(animationGraph->States)) {
		// Error: Cannot find the state
		return;
	}
	
	POMDOG_ASSERT(iter->Tree);
	nextAnimation.Node = std::shared_ptr<AnimationNode>(animationGraph, iter->Tree.get());
	nextAnimation.Name = stateName;

	auto crossFade = std::make_shared<AnimationCrossFadeNode>(currentAnimation, nextAnimation, transitionDuration, time);

	currentAnimation.Node = crossFade;
	time = AnimationTimeInterval::zero();
}
//-----------------------------------------------------------------------
void SkeletonAnimator::Play(std::string const& stateName)
{
	POMDOG_ASSERT(animationGraph);
	POMDOG_ASSERT(!animationGraph->States.empty());

	auto iter = FindState(animationGraph->States, stateName);
	POMDOG_ASSERT(iter != std::end(animationGraph->States));
	
	if (iter == std::end(animationGraph->States)) {
		// Error: Cannot find the state
		return;
	}
	
	POMDOG_ASSERT(iter->Tree);
	currentAnimation.Node = std::shared_ptr<AnimationNode>(animationGraph, iter->Tree.get());
	currentAnimation.Name = stateName;
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
std::string SkeletonAnimator::GetCurrentStateName() const
{
	return currentAnimation.Name;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
