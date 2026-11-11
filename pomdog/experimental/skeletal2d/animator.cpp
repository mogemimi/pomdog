// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/animator.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/skeletal2d/animation_state.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_cross_fade_node.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_graph.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_graph_weight_collection.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_node.h"
#include "pomdog/experimental/skeletal2d/blendtrees/weight_blending_helper.h"
#include "pomdog/experimental/skeletal2d/skeleton.h"
#include "pomdog/experimental/skeletal2d/skeleton_helper.h"
#include "pomdog/experimental/skeletal2d/skeleton_pose.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {
namespace {

auto FindState(
    const std::vector<AnimationGraphState>& states,
    u32 stateKey)
    -> decltype(states.begin())
{
    // NOTE: A graph has a few states, so a linear search is enough.
    return std::find_if(std::begin(states), std::end(states),
        [&stateKey](const AnimationGraphState& state) {
            return state.Key == stateKey;
        });
}

AnimationTimeInterval
WrapTime(const AnimationTimeInterval& source, const AnimationTimeInterval& max)
{
    auto time = source;
    while (time > max) {
        time -= max;
    }

    POMDOG_ASSERT(time >= AnimationTimeInterval::zero());
    POMDOG_ASSERT(time <= max);

    return time;
}

} // namespace

// MARK: - Animator::Impl class

class Animator::Impl final {
public:
    Impl(
        const std::shared_ptr<Skeleton>& skeleton,
        const std::shared_ptr<skeletal2d::SkeletonPose>& skeletonPose,
        const std::shared_ptr<AnimationGraph>& animationGraph);

    void Update(const Duration& frameDuration);

    void CrossFade(u32 stateKey, const Duration& transitionDuration);

    void Play(u32 stateKey);

    void SetFloat(u32 parameterKey, f32 value);

    void SetBool(u32 parameterKey, bool value);

public:
    detail::AnimationGraphWeightCollection graphWeights;
    std::shared_ptr<Skeleton> skeleton;
    std::shared_ptr<skeletal2d::SkeletonPose> skeletonPose;
    detail::SkeletonAnimationState currentAnimation;
    std::optional<detail::SkeletonAnimationState> nextAnimation;
    std::shared_ptr<AnimationGraph const> animationGraph;
    AnimationTimeInterval time;
    float playbackRate;
};

Animator::Impl::Impl(
    const std::shared_ptr<Skeleton>& skeletonIn,
    const std::shared_ptr<skeletal2d::SkeletonPose>& skeletonPoseIn,
    const std::shared_ptr<AnimationGraph>& animationGraphIn)
    : skeleton(skeletonIn)
    , skeletonPose(skeletonPoseIn)
    , animationGraph(animationGraphIn)
    , time(AnimationTimeInterval::zero())
    , playbackRate(1.0f)
{
    POMDOG_ASSERT(skeleton != nullptr);
    POMDOG_ASSERT(animationGraph != nullptr);

    graphWeights.Reserve(animationGraph->Inputs.size());
    for (auto& parameter : animationGraph->Inputs) {
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

    currentAnimation.Key = animationGraph->States.front().Key;
    currentAnimation.Node = std::shared_ptr<AnimationNode>(animationGraph,
        animationGraph->States.front().Tree.get());
}

void Animator::Impl::Update(const Duration& frameDuration)
{
    // (1) Update time:
    {
        time = time + frameDuration * playbackRate;

        POMDOG_ASSERT(currentAnimation.Node);
        if (time < AnimationTimeInterval::zero()) {
            time = currentAnimation.Node->GetLength();
        }
        else if (time > currentAnimation.Node->GetLength()) {
            if (nextAnimation) {
                // transit
                currentAnimation.Node = nextAnimation->Node;
                currentAnimation.Key = nextAnimation->Key;
                nextAnimation = std::nullopt;

                time = WrapTime(time, currentAnimation.Node->GetLength());
                POMDOG_ASSERT(time >= AnimationTimeInterval::zero());
                POMDOG_ASSERT(time <= currentAnimation.Node->GetLength());
            }
            else {
                //POMDOG_ASSERT(loop);
                time = AnimationTimeInterval::zero();
            }
        }
    }

    // (2) Pose extraction, and (3) Pose blending:
    POMDOG_ASSERT(currentAnimation.Node);
    currentAnimation.Node->Calculate(time, graphWeights, *skeleton, *skeletonPose, nullptr);
}

void Animator::Impl::CrossFade(u32 stateKey, const Duration& transitionDuration)
{
    if (nextAnimation) {
        return;
    }

    POMDOG_ASSERT(transitionDuration > Duration::zero());
    POMDOG_ASSERT(!std::isnan(transitionDuration.count()));

    auto iter = FindState(animationGraph->States, stateKey);
    POMDOG_ASSERT(iter != std::end(animationGraph->States));

    if (iter == std::end(animationGraph->States)) {
        // Error: Cannot find the state
        return;
    }

    POMDOG_ASSERT(iter->Tree);
    nextAnimation = detail::SkeletonAnimationState{};
    nextAnimation->Node = std::shared_ptr<AnimationNode>(animationGraph, iter->Tree.get());
    nextAnimation->Key = stateKey;

    auto crossFade = std::make_shared<detail::AnimationCrossFadeNode>(currentAnimation, *nextAnimation, transitionDuration, time);

    currentAnimation.Node = crossFade;
    time = AnimationTimeInterval::zero();
}

void Animator::Impl::Play(u32 stateKey)
{
    POMDOG_ASSERT(animationGraph);
    POMDOG_ASSERT(!animationGraph->States.empty());

    auto iter = FindState(animationGraph->States, stateKey);
    POMDOG_ASSERT(iter != std::end(animationGraph->States));

    if (iter == std::end(animationGraph->States)) {
        // Error: Cannot find the state
        return;
    }

    POMDOG_ASSERT(iter->Tree);
    currentAnimation.Node = std::shared_ptr<AnimationNode>(animationGraph, iter->Tree.get());
    currentAnimation.Key = stateKey;
    time = AnimationTimeInterval::zero();
}

void Animator::Impl::SetFloat(u32 parameterKey, f32 value)
{
    POMDOG_ASSERT(!std::isnan(value));
    POMDOG_ASSERT(animationGraph);
    if (auto index = animationGraph->FindParameter(parameterKey); index != std::nullopt) {
        graphWeights.SetValue(*index, value);
    }
}

void Animator::Impl::SetBool(u32 parameterKey, bool value)
{
    POMDOG_ASSERT(animationGraph);
    if (auto index = animationGraph->FindParameter(parameterKey); index != std::nullopt) {
        graphWeights.SetValue(*index, value);
    }
}

// MARK: - Animator class

Animator::Animator(
    const std::shared_ptr<Skeleton>& skeleton,
    const std::shared_ptr<skeletal2d::SkeletonPose>& skeletonPose,
    const std::shared_ptr<AnimationGraph>& animationGraph)
    : impl(std::make_unique<Impl>(skeleton, skeletonPose, animationGraph))
{
}

Animator::~Animator() = default;

void Animator::Update(const Duration& frameDuration)
{
    POMDOG_ASSERT(impl);
    impl->Update(frameDuration);
}

void Animator::CrossFade(u32 stateKey, const Duration& transitionDuration)
{
    POMDOG_ASSERT(impl);
    impl->CrossFade(stateKey, transitionDuration);
}

void Animator::Play(u32 stateKey)
{
    POMDOG_ASSERT(impl);
    impl->Play(stateKey);
}

float Animator::GetPlaybackRate() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->playbackRate;
}

void Animator::SetPlaybackRate(float playbackRateIn) noexcept
{
    POMDOG_ASSERT(impl);
    impl->playbackRate = playbackRateIn;
}

void Animator::SetFloat(u32 parameterKey, f32 value)
{
    POMDOG_ASSERT(impl);
    impl->SetFloat(parameterKey, value);
}

void Animator::SetBool(u32 parameterKey, bool value)
{
    POMDOG_ASSERT(impl);
    impl->SetBool(parameterKey, value);
}

u32 Animator::GetCurrentStateKey() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->currentAnimation.Key;
}

} // namespace pomdog::skeletal2d
