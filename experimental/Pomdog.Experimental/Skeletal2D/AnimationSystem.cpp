// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "AnimationSystem.hpp"
#include "AnimationClip.hpp"
#include "AnimationState.hpp"
#include "JointIndex.hpp"
#include "Skeleton.hpp"
#include "SkeletonPose.hpp"
#include "SkeletonHelper.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <vector>
#include <algorithm>

namespace Pomdog {
namespace {

class SkeletonAnimationContext {
public:
    std::shared_ptr<Pomdog::AnimationState> AnimationState;
    std::shared_ptr<Pomdog::Skeleton const> Skeleton;
    std::shared_ptr<Pomdog::SkeletonPose> SkeletonPose;
};

} // unnamed namespace

// MARK: - AnimationSystem::Impl class

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

void AnimationSystem::Impl::Add(std::shared_ptr<AnimationState> const& state,
    std::shared_ptr<Skeleton const> const& skeleton,
    std::shared_ptr<SkeletonPose> const& skeletonPose)
{
    skeletonAnimations.push_back({state, skeleton, skeletonPose});
}

void AnimationSystem::Impl::Remove(std::shared_ptr<AnimationState> const& state)
{
    skeletonAnimations.erase(std::remove_if(std::begin(skeletonAnimations), std::end(skeletonAnimations),
        [&state](SkeletonAnimationContext const& context){ return context.AnimationState == state; }),
        std::end(skeletonAnimations));
}

void AnimationSystem::Impl::Update(GameClock const& clock)
{
    for (auto & animationContext: skeletonAnimations)
    {
        // (1) Update time:
        auto & state = *animationContext.AnimationState;
        state.Update(clock.GetFrameDuration());

        // (2) Pose extraction:
        auto & clip = state.Clip();
        auto & skeleton = *animationContext.Skeleton;
        auto & skeletonPose = *animationContext.SkeletonPose;

        clip->Apply(state.Time(), skeleton, skeletonPose);

        // (3) Pose blending:
        ///@todo Not implemented
    }
}

// MARK: - AnimationSystem class

AnimationSystem::AnimationSystem()
    : impl(std::make_unique<Impl>())
{
}

AnimationSystem::~AnimationSystem() = default;

void AnimationSystem::Update(GameClock const& clock)
{
    POMDOG_ASSERT(impl);
    impl->Update(clock);
}

void AnimationSystem::Add(std::shared_ptr<AnimationState> const& state,
    std::shared_ptr<Skeleton const> const& skeleton,
    std::shared_ptr<SkeletonPose> const& skeletonPose)
{
    POMDOG_ASSERT(impl);
    impl->Add(state, skeleton, skeletonPose);
}

void AnimationSystem::Remove(std::shared_ptr<AnimationState> const& state)
{
    POMDOG_ASSERT(impl);
    impl->Remove(state);
}

} // namespace Pomdog
