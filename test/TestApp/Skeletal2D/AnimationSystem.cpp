//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "AnimationSystem.hpp"
#include <vector>
#include <algorithm>
#include <functional>
#include <Pomdog/Math/Matrix4x4.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include "AnimationClip.hpp"
#include "AnimationState.hpp"
#include "JointIndex.hpp"
#include "Skeleton.hpp"
#include "SkeletonPose.hpp"

namespace Pomdog {
namespace {

class SkeletonAnimationContext {
public:
	std::shared_ptr<AnimationState> AnimationState;
	std::shared_ptr<Skeleton const> Skeleton;
	std::shared_ptr<SkeletonPose> SkeletonPose;
};
//-----------------------------------------------------------------------
static void Traverse(
	Skeleton const& skeleton, JointIndex const& boneIndex,
	std::function<void(Joint const& target)> const& traverser)
{
	POMDOG_ASSERT(boneIndex);
	auto & bone = skeleton.Joints(boneIndex);

	POMDOG_ASSERT(bone.Parent);
	traverser(bone);
	
	if (bone.FirstChild) {
		Traverse(skeleton, bone.FirstChild, traverser);
	}
	if (bone.Sibling) {
		Traverse(skeleton, bone.Sibling, traverser);
	}
}
//-----------------------------------------------------------------------
static void Traverse(Skeleton const& skeleton,
	std::function<void(Joint const& target)> const& traverser)
{
	POMDOG_ASSERT(skeleton.JointCount() > 0);
	POMDOG_ASSERT(skeleton.Root().Index);
	if (skeleton.Root().FirstChild)
	{
		Traverse(skeleton, skeleton.Root().FirstChild, traverser);
	}
}

}// unnamed namespace
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - AnimationSystem::Impl class
#endif
//-----------------------------------------------------------------------
class AnimationSystem::Impl {
public:
	std::vector<SkeletonAnimationContext> skeletonAnimations;
	
public:
	void Add(std::shared_ptr<AnimationState> const& state,
		std::shared_ptr<Skeleton const> const& skeleton,
		std::shared_ptr<SkeletonPose> const& skeletonPose);
	
	void Remove(std::shared_ptr<AnimationState> const& state);

	void Update(GameClock const& clock);
};
//-----------------------------------------------------------------------
void AnimationSystem::Impl::Add(std::shared_ptr<AnimationState> const& state,
	std::shared_ptr<Skeleton const> const& skeleton,
	std::shared_ptr<SkeletonPose> const& skeletonPose)
{
	skeletonAnimations.push_back({state, skeleton, skeletonPose});
}
//-----------------------------------------------------------------------
void AnimationSystem::Impl::Remove(std::shared_ptr<AnimationState> const& state)
{
	skeletonAnimations.erase(std::remove_if(std::begin(skeletonAnimations), std::end(skeletonAnimations),
		[&state](SkeletonAnimationContext const& context){ return context.AnimationState == state; }),
		std::end(skeletonAnimations));
}
//-----------------------------------------------------------------------
void AnimationSystem::Impl::Update(GameClock const& clock)
{
	for (auto & animationContext: skeletonAnimations)
	{
		// (1) Update time:
		auto & state = *animationContext.AnimationState;
	
		auto time = state.Time() + clock.FrameDuration() * state.PlaybackRate();
		if (time < DurationSeconds(0)) {
			time = state.Length();
		}
		else if (time > state.Length()) {
			POMDOG_ASSERT(state.Loop());
			time = DurationSeconds(0);
		}
		state.Time(time);

		// (2) Pose extraction:
		auto & clip = state.Clip();
		auto & skeleton = *animationContext.Skeleton;
		auto & skeletonPose = *animationContext.SkeletonPose;
		
		clip->Apply(state.Time(), skeleton, skeletonPose);
		
		// (3) Pose blending:
		///@todo Not implemented
		
		// (4) Global pose generation:
		{
			auto bone = skeleton.Root();
			POMDOG_ASSERT(*bone.Index < skeletonPose.LocalPose.size());
			auto & pose = skeletonPose.LocalPose[*bone.Index];
		
			Matrix4x4 matrix = Matrix4x4::CreateScale(pose.Scale);
			matrix *= Matrix4x4::CreateRotationZ(pose.Rotation);
			matrix *= Matrix4x4::CreateTranslation({pose.Translate, 0.0f});
		
			POMDOG_ASSERT(*bone.Index < skeletonPose.GlobalPose.size());
			skeletonPose.GlobalPose[*bone.Index] = Matrix4x4::Identity;
		}

		Traverse(skeleton, [&](Joint const& bone)
		{
			POMDOG_ASSERT(*bone.Index < skeletonPose.LocalPose.size());
			auto & pose = skeletonPose.LocalPose[*bone.Index];
			
			POMDOG_ASSERT(bone.Parent);
			POMDOG_ASSERT(*bone.Index < skeletonPose.GlobalPose.size());
			auto & parentMatrix = skeletonPose.GlobalPose[*bone.Parent];

			Matrix4x4 matrix = Matrix4x4::CreateScale(pose.Scale);
			matrix *= Matrix4x4::CreateRotationZ(pose.Rotation);
			matrix *= Matrix4x4::CreateTranslation({pose.Translate, 0.0f});
			matrix *= parentMatrix;

			POMDOG_ASSERT(*bone.Index < skeletonPose.GlobalPose.size());
			skeletonPose.GlobalPose[*bone.Index] = matrix;
		});
	}
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - AnimationSystem class
#endif
//-----------------------------------------------------------------------
AnimationSystem::AnimationSystem()
	: impl(new Impl())
{
}
//-----------------------------------------------------------------------
AnimationSystem::~AnimationSystem() = default;
//-----------------------------------------------------------------------
void AnimationSystem::Update(GameClock const& clock)
{
	POMDOG_ASSERT(impl);
	impl->Update(clock);
}
//-----------------------------------------------------------------------
void AnimationSystem::Add(std::shared_ptr<AnimationState> const& state,
	std::shared_ptr<Skeleton const> const& skeleton,
	std::shared_ptr<SkeletonPose> const& skeletonPose)
{
	POMDOG_ASSERT(impl);
	impl->Add(state, skeleton, skeletonPose);
}
//-----------------------------------------------------------------------
void AnimationSystem::Remove(std::shared_ptr<AnimationState> const& state)
{
	POMDOG_ASSERT(impl);
	impl->Remove(state);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
