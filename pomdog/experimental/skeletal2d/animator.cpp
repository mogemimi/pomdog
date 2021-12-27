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
    const std::string& stateName)
    -> decltype(states.begin())
{
    return std::find_if(std::begin(states), std::end(states),
        [&stateName](const AnimationGraphState& state) {
            return state.Name == stateName;
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

    void CrossFade(const std::string& stateName, const Duration& transitionDuration);

    void Play(const std::string& stateName);

    void SetFloat(const std::string& name, float value);

    void SetBool(const std::string& name, bool value);

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

    currentAnimation.Name = animationGraph->States.front().Name;
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
                currentAnimation.Name = nextAnimation->Name;
                nextAnimation->Node.reset();
                nextAnimation->Name.clear();
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

void Animator::Impl::CrossFade(const std::string& stateName, const Duration& transitionDuration)
{
    if (nextAnimation) {
        return;
    }

    POMDOG_ASSERT(transitionDuration > Duration::zero());
    POMDOG_ASSERT(!std::isnan(transitionDuration.count()));

    auto iter = FindState(animationGraph->States, stateName);
    POMDOG_ASSERT(iter != std::end(animationGraph->States));

    if (iter == std::end(animationGraph->States)) {
        // Error: Cannot find the state
        return;
    }

    POMDOG_ASSERT(iter->Tree);
    nextAnimation = detail::SkeletonAnimationState{};
    nextAnimation->Node = std::shared_ptr<AnimationNode>(animationGraph, iter->Tree.get());
    nextAnimation->Name = stateName;

    auto crossFade = std::make_shared<detail::AnimationCrossFadeNode>(currentAnimation, *nextAnimation, transitionDuration, time);

    currentAnimation.Node = crossFade;
    time = AnimationTimeInterval::zero();
}

void Animator::Impl::Play(const std::string& stateName)
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

void Animator::Impl::SetFloat(const std::string& name, float value)
{
    POMDOG_ASSERT(!std::isnan(value));
    POMDOG_ASSERT(animationGraph);
    if (auto index = animationGraph->FindParameter(name); index != std::nullopt) {
        graphWeights.SetValue(*index, value);
    }
}

void Animator::Impl::SetBool(const std::string& name, bool value)
{
    POMDOG_ASSERT(animationGraph);
    if (auto index = animationGraph->FindParameter(name); index != std::nullopt) {
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

void Animator::CrossFade(const std::string& state, const Duration& transitionDuration)
{
    POMDOG_ASSERT(impl);
    impl->CrossFade(state, transitionDuration);
}

void Animator::Play(const std::string& state)
{
    POMDOG_ASSERT(impl);
    impl->Play(state);
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

void Animator::SetFloat(const std::string& name, float value)
{
    POMDOG_ASSERT(impl);
    impl->SetFloat(name, value);
}

void Animator::SetBool(const std::string& name, bool value)
{
    POMDOG_ASSERT(impl);
    impl->SetBool(name, value);
}

std::string Animator::GetCurrentStateName() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->currentAnimation.Name;
}

} // namespace pomdog::skeletal2d
