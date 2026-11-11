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
    std::shared_ptr<skeletal2d::AnimationState> animationState;
    std::shared_ptr<skeletal2d::Skeleton const> skeleton;
    std::shared_ptr<skeletal2d::SkeletonPose> skeletonPose;
    std::shared_ptr<skeletal2d::Skin> skin;
};

} // namespace

// MARK: - AnimationSystem::Impl class

class AnimationSystem::Impl final {
public:
    std::vector<SkeletonAnimationContext> skeletonAnimations;

public:
    void add(
        const std::shared_ptr<AnimationState>& state,
        const std::shared_ptr<Skeleton const>& skeleton,
        const std::shared_ptr<SkeletonPose>& skeletonPose,
        const std::shared_ptr<Skin>& skin);

    void remove(const std::shared_ptr<AnimationState>& state);

    void update(const GameClock& clock);
};

void AnimationSystem::Impl::add(
    const std::shared_ptr<AnimationState>& state,
    const std::shared_ptr<Skeleton const>& skeleton,
    const std::shared_ptr<SkeletonPose>& skeletonPose,
    const std::shared_ptr<Skin>& skin)
{
    skeletonAnimations.push_back({state, skeleton, skeletonPose, skin});
}

void AnimationSystem::Impl::remove(const std::shared_ptr<AnimationState>& state)
{
    skeletonAnimations.erase(
        std::remove_if(
            std::begin(skeletonAnimations),
            std::end(skeletonAnimations),
            [&state](const SkeletonAnimationContext& context) { return context.animationState == state; }),
        std::end(skeletonAnimations));
}

void AnimationSystem::Impl::update(const GameClock& clock)
{
    for (auto& animationContext : skeletonAnimations) {
        // (1) Update time:
        auto& state = *animationContext.animationState;
        state.update(clock.getFrameDuration());

        // (2) Pose extraction:
        auto& clip = state.getClip();
        auto& skeleton = *animationContext.skeleton;
        auto& skeletonPose = *animationContext.skeletonPose;
        auto& skin = animationContext.skin;

        clip->apply(state.getTime(), skeleton, skeletonPose, skin.get());

        // (3) Pose blending:
        ///@todo Not implemented
    }
}

// MARK: - AnimationSystem class

AnimationSystem::AnimationSystem()
    : impl_(std::make_unique<Impl>())
{
}

AnimationSystem::~AnimationSystem() = default;

void AnimationSystem::update(const GameClock& clock)
{
    POMDOG_ASSERT(impl_);
    impl_->update(clock);
}

void AnimationSystem::add(
    const std::shared_ptr<AnimationState>& state,
    const std::shared_ptr<Skeleton const>& skeleton,
    const std::shared_ptr<SkeletonPose>& skeletonPose,
    const std::shared_ptr<Skin>& skin)
{
    POMDOG_ASSERT(impl_);
    impl_->add(state, skeleton, skeletonPose, skin);
}

void AnimationSystem::remove(const std::shared_ptr<AnimationState>& state)
{
    POMDOG_ASSERT(impl_);
    impl_->remove(state);
}

} // namespace pomdog::skeletal2d
