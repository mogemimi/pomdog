// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Skeletal2D/AnimationSystem.hpp"
#include "Pomdog/Experimental/Skeletal2D/AnimationClip.hpp"
#include "Pomdog/Experimental/Skeletal2D/AnimationState.hpp"
#include "Pomdog/Experimental/Skeletal2D/JointIndex.hpp"
#include "Pomdog/Experimental/Skeletal2D/Skeleton.hpp"
#include "Pomdog/Experimental/Skeletal2D/SkeletonHelper.hpp"
#include "Pomdog/Experimental/Skeletal2D/SkeletonPose.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <vector>

namespace Pomdog::Skeletal2D {
namespace {

class SkeletonAnimationContext final {
public:
    std::shared_ptr<Skeletal2D::AnimationState> AnimationState;
    std::shared_ptr<Skeletal2D::Skeleton const> Skeleton;
    std::shared_ptr<Skeletal2D::SkeletonPose> SkeletonPose;
    std::shared_ptr<Skeletal2D::Skin> Skin;
};

} // namespace

// MARK: - AnimationSystem::Impl class

class AnimationSystem::Impl final {
public:
    std::vector<SkeletonAnimationContext> skeletonAnimations;

public:
    void Add(
        const std::shared_ptr<AnimationState>& state,
        const std::shared_ptr<Skeleton const>& skeleton,
        const std::shared_ptr<SkeletonPose>& skeletonPose,
        const std::shared_ptr<Skin>& skin);

    void Remove(const std::shared_ptr<AnimationState>& state);

    void Update(const GameClock& clock);
};

void AnimationSystem::Impl::Add(
    const std::shared_ptr<AnimationState>& state,
    const std::shared_ptr<Skeleton const>& skeleton,
    const std::shared_ptr<SkeletonPose>& skeletonPose,
    const std::shared_ptr<Skin>& skin)
{
    skeletonAnimations.push_back({state, skeleton, skeletonPose, skin});
}

void AnimationSystem::Impl::Remove(const std::shared_ptr<AnimationState>& state)
{
    skeletonAnimations.erase(std::remove_if(std::begin(skeletonAnimations), std::end(skeletonAnimations),
        [&state](const SkeletonAnimationContext& context) { return context.AnimationState == state; }),
        std::end(skeletonAnimations));
}

void AnimationSystem::Impl::Update(const GameClock& clock)
{
    for (auto& animationContext : skeletonAnimations) {
        // (1) Update time:
        auto& state = *animationContext.AnimationState;
        state.Update(clock.GetFrameDuration());

        // (2) Pose extraction:
        auto& clip = state.GetClip();
        auto& skeleton = *animationContext.Skeleton;
        auto& skeletonPose = *animationContext.SkeletonPose;
        auto& skin = animationContext.Skin;

        clip->Apply(state.GetTime(), skeleton, skeletonPose, skin.get());

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

void AnimationSystem::Update(const GameClock& clock)
{
    POMDOG_ASSERT(impl);
    impl->Update(clock);
}

void AnimationSystem::Add(
    const std::shared_ptr<AnimationState>& state,
    const std::shared_ptr<Skeleton const>& skeleton,
    const std::shared_ptr<SkeletonPose>& skeletonPose,
    const std::shared_ptr<Skin>& skin)
{
    POMDOG_ASSERT(impl);
    impl->Add(state, skeleton, skeletonPose, skin);
}

void AnimationSystem::Remove(const std::shared_ptr<AnimationState>& state)
{
    POMDOG_ASSERT(impl);
    impl->Remove(state);
}

} // namespace Pomdog::Skeletal2D
