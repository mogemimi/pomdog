// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Animator.hpp"
#include "Pomdog.Experimental/Skeletal2D/detail/WeightBlendingHelper.hpp"
#include "Pomdog.Experimental/Skeletal2D/detail/AnimationGraphWeightCollection.hpp"
#include "Pomdog.Experimental/Skeletal2D/AnimationNode.hpp"
#include "Pomdog.Experimental/Skeletal2D/AnimationState.hpp"
#include "Pomdog.Experimental/Skeletal2D/AnimationGraph.hpp"
#include "Pomdog.Experimental/Skeletal2D/Skeleton.hpp"
#include "Pomdog.Experimental/Skeletal2D/SkeletonHelper.hpp"
#include "Pomdog.Experimental/Skeletal2D/SkeletonTransform.hpp"
#include <cmath>
#include <optional>

namespace Pomdog {
namespace {

struct SkeletonAnimationState {
    std::shared_ptr<AnimationNode const> Node;
    std::string Name;
};

auto FindState(std::vector<AnimationGraphState> const& states, std::string const& stateName)
    ->decltype(states.begin())
{
    return std::find_if(std::begin(states), std::end(states),
        [&stateName](AnimationGraphState const& state) {
            return state.Name == stateName;
        });
}

AnimationTimeInterval WrapTime(AnimationTimeInterval const& source, AnimationTimeInterval const& max)
{
    auto time = source;
    while (time > max) {
        time -= max;
    }

    POMDOG_ASSERT(time >= AnimationTimeInterval::zero());
    POMDOG_ASSERT(time <= max);

    return time;
}

class AnimationCrossFadeNode final : public AnimationNode {
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
        Detail::Skeletal2D::AnimationGraphWeightCollection const& weights,
        Skeleton const& skeleton,
        SkeletonPose & skeletonPose) const override
    {
        auto sourcePose1 = SkeletonPose::CreateBindPose(skeleton);
        auto sourcePose2 = SkeletonPose::CreateBindPose(skeleton);

        {
            auto sourceTime = WrapTime(currentAnimationStartTime + time, currentAnimation.Node->Length());

            POMDOG_ASSERT(sourceTime >= AnimationTimeInterval::zero());
            POMDOG_ASSERT(sourceTime <= currentAnimation.Node->Length());

            POMDOG_ASSERT(currentAnimation.Node);
            currentAnimation.Node->Calculate(sourceTime, weights, skeleton, sourcePose1);
        }
        {
            auto sourceTime = WrapTime(time, nextAnimation.Node->Length());

            POMDOG_ASSERT(sourceTime >= AnimationTimeInterval::zero());
            POMDOG_ASSERT(sourceTime <= nextAnimation.Node->Length());

            POMDOG_ASSERT(nextAnimation.Node);
            nextAnimation.Node->Calculate(sourceTime, weights, skeleton, sourcePose2);
        }

        POMDOG_ASSERT(transitionDuration.count() > 0);
        float weight = (time / transitionDuration.count()).count();
        POMDOG_ASSERT(weight >= 0.0f);
        POMDOG_ASSERT(weight <= 1.0f);

        using Detail::Skeletal2D::WeightBlendingHelper;
        WeightBlendingHelper::Lerp(sourcePose1.JointPoses, sourcePose2.JointPoses, weight, skeletonPose.JointPoses);
    }

    AnimationTimeInterval Length() const override
    {
        return transitionDuration;
    }
};

} // unnamed namespace

// MARK: - Animator::Impl class

class Animator::Impl final {
public:
    Impl(
        std::shared_ptr<Skeleton> const& skeleton,
        std::shared_ptr<SkeletonTransform> const& skeletonTransform,
        std::shared_ptr<AnimationGraph> const& animationGraph);

    void Update(Duration const& frameDuration);

    void CrossFade(std::string const& stateName, Duration const& transitionDuration);

    void Play(std::string const& stateName);

    void SetFloat(std::string const& name, float value);

    void SetBool(std::string const& name, bool value);

public:
    Detail::Skeletal2D::AnimationGraphWeightCollection graphWeights;
    std::shared_ptr<Skeleton> skeleton;
    std::shared_ptr<SkeletonTransform> skeletonTransform;
    SkeletonAnimationState currentAnimation;
    std::optional<SkeletonAnimationState> nextAnimation;
    std::shared_ptr<AnimationGraph const> animationGraph;
    AnimationTimeInterval time;
    float playbackRate;
};

Animator::Impl::Impl(std::shared_ptr<Skeleton> const& skeletonIn,
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

void Animator::Impl::Update(Duration const& frameDuration)
{
    // (1) Update time:
    {
        time = time + frameDuration * playbackRate;

        POMDOG_ASSERT(currentAnimation.Node);
        if (time < AnimationTimeInterval::zero()) {
            time = currentAnimation.Node->Length();
        }
        else if (time > currentAnimation.Node->Length()) {
            if (nextAnimation) {
                // transit
                currentAnimation.Node = nextAnimation->Node;
                currentAnimation.Name = nextAnimation->Name;
                nextAnimation->Node.reset();
                nextAnimation->Name.clear();
                nextAnimation = std::nullopt;

                time = WrapTime(time, currentAnimation.Node->Length());
                POMDOG_ASSERT(time >= AnimationTimeInterval::zero());
                POMDOG_ASSERT(time <= currentAnimation.Node->Length());
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

void Animator::Impl::CrossFade(std::string const& stateName, Duration const& transitionDuration)
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
    nextAnimation = SkeletonAnimationState{};
    nextAnimation->Node = std::shared_ptr<AnimationNode>(animationGraph, iter->Tree.get());
    nextAnimation->Name = stateName;

    auto crossFade = std::make_shared<AnimationCrossFadeNode>(currentAnimation, *nextAnimation, transitionDuration, time);

    currentAnimation.Node = crossFade;
    time = AnimationTimeInterval::zero();
}

void Animator::Impl::Play(std::string const& stateName)
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

void Animator::Impl::SetFloat(std::string const& name, float value)
{
    POMDOG_ASSERT(!std::isnan(value));
    POMDOG_ASSERT(animationGraph);
    if (auto index = animationGraph->FindParameter(name)) {
        graphWeights.SetValue(*index, value);
    }
}

void Animator::Impl::SetBool(std::string const& name, bool value)
{
    POMDOG_ASSERT(animationGraph);
    if (auto index = animationGraph->FindParameter(name)) {
        graphWeights.SetValue(*index, value);
    }
}

// MARK: - Animator class

Animator::Animator(std::shared_ptr<Skeleton> const& skeleton,
    std::shared_ptr<SkeletonTransform> const& skeletonTransform,
    std::shared_ptr<AnimationGraph> const& animationGraph)
    : impl(std::make_unique<Impl>(skeleton, skeletonTransform, animationGraph))
{}

Animator::~Animator() = default;

void Animator::Update(Duration const& frameDuration)
{
    POMDOG_ASSERT(impl);
    impl->Update(frameDuration);
}

void Animator::CrossFade(std::string const& state, Duration const& transitionDuration)
{
    POMDOG_ASSERT(impl);
    impl->CrossFade(state, transitionDuration);
}

void Animator::Play(std::string const& state)
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

void Animator::SetFloat(std::string const& name, float value)
{
    POMDOG_ASSERT(impl);
    impl->SetFloat(name, value);
}

void Animator::SetBool(std::string const& name, bool value)
{
    POMDOG_ASSERT(impl);
    impl->SetBool(name, value);
}

std::string Animator::GetCurrentStateName() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->currentAnimation.Name;
}

} // namespace Pomdog
