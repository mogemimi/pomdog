// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/animation_system.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/skeletal2d/animation_clip.h"
#include "pomdog/experimental/skeletal2d/animation_state.h"
#include "pomdog/experimental/skeletal2d/joint_index.h"
#include "pomdog/experimental/skeletal2d/skeleton.h"
#include "pomdog/experimental/skeletal2d/skeleton_helper.h"
#include "pomdog/experimental/skeletal2d/skeleton_pose.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {
namespace {

class SkeletonAnimationContext final {
public:
    std::shared_ptr<skeletal2d::AnimationState> AnimationState;
    std::shared_ptr<skeletal2d::Skeleton const> Skeleton;
    std::shared_ptr<skeletal2d::SkeletonPose> SkeletonPose;
    std::shared_ptr<skeletal2d::Skin> Skin;
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
    skeletonAnimations.erase(
        std::remove_if(
            std::begin(skeletonAnimations),
            std::end(skeletonAnimations),
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

} // namespace pomdog::skeletal2d
