// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/animator.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
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

auto findState(
    const std::vector<AnimationGraphState>& states,
    u32 stateKey)
    -> decltype(states.begin())
{
    // NOTE: A graph has a few states, so a linear search is enough.
    return std::find_if(std::begin(states), std::end(states),
        [&stateKey](const AnimationGraphState& state) {
            return state.key == stateKey;
        });
}

AnimationTimeInterval
wrapTime(const AnimationTimeInterval& source, const AnimationTimeInterval& max)
{
    // NOTE: A state whose clips have no keys has a length of zero, so every
    // time of it is the beginning. Subtracting the length would never end.
    if (max <= AnimationTimeInterval::zero()) {
        return AnimationTimeInterval::zero();
    }

    // NOTE: std::fmod() takes constant time, and also ends when subtracting
    // a short length from a long time no longer changes the time.
    auto time = source;
    if (time > max) {
        time = AnimationTimeInterval{std::fmod(time.count(), max.count())};
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

    void update(const Duration& frameDuration);

    void crossFade(u32 stateKey, const Duration& transitionDuration);

    void play(u32 stateKey);

    void setFloat(u32 parameterKey, f32 value);

    void setBool(u32 parameterKey, bool value);

public:
    detail::AnimationGraphWeightCollection graphWeights;
    std::shared_ptr<Skeleton> skeleton;
    std::shared_ptr<skeletal2d::SkeletonPose> skeletonPose;
    detail::SkeletonAnimationState currentAnimation;
    std::optional<detail::SkeletonAnimationState> nextAnimation;
    std::shared_ptr<AnimationGraph const> animationGraph;
    AnimationTimeInterval time;
    f32 playbackRate;
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

    graphWeights.reserve(animationGraph->inputs.size());
    for (auto& parameter : animationGraph->inputs) {
        switch (parameter.type) {
        case AnimationBlendInputType::Float:
            graphWeights.addFloat();
            break;
        case AnimationBlendInputType::Bool:
            graphWeights.addBool();
            break;
        }
    }

    POMDOG_ASSERT(animationGraph);
    POMDOG_ASSERT(!animationGraph->states.empty());
    POMDOG_ASSERT(animationGraph->states.front().tree);

    currentAnimation.key = animationGraph->states.front().key;
    currentAnimation.node = std::shared_ptr<AnimationNode>(animationGraph,
        animationGraph->states.front().tree.get());
}

void Animator::Impl::update(const Duration& frameDuration)
{
    // (1) Update time:
    {
        time = time + frameDuration * playbackRate;

        POMDOG_ASSERT(currentAnimation.node);
        if (time < AnimationTimeInterval::zero()) {
            time = currentAnimation.node->getLength();
        }
        else if (time > currentAnimation.node->getLength()) {
            if (nextAnimation) {
                // transit
                currentAnimation.node = nextAnimation->node;
                currentAnimation.key = nextAnimation->key;
                nextAnimation = std::nullopt;

                time = wrapTime(time, currentAnimation.node->getLength());
                POMDOG_ASSERT(time >= AnimationTimeInterval::zero());
                POMDOG_ASSERT(time <= currentAnimation.node->getLength());
            }
            else {
                //POMDOG_ASSERT(loop);
                time = AnimationTimeInterval::zero();
            }
        }
    }

    // (2) Pose extraction, and (3) Pose blending:
    POMDOG_ASSERT(currentAnimation.node);
    currentAnimation.node->calculate(time, graphWeights, *skeleton, *skeletonPose, nullptr);
}

void Animator::Impl::crossFade(u32 stateKey, const Duration& transitionDuration)
{
    if (nextAnimation) {
        return;
    }

    POMDOG_ASSERT(transitionDuration > Duration::zero());
    POMDOG_ASSERT(!std::isnan(transitionDuration.count()));

    auto iter = findState(animationGraph->states, stateKey);
    POMDOG_ASSERT(iter != std::end(animationGraph->states));

    if (iter == std::end(animationGraph->states)) {
        // Error: Cannot find the state
        return;
    }

    POMDOG_ASSERT(iter->tree);
    nextAnimation = detail::SkeletonAnimationState{};
    nextAnimation->node = std::shared_ptr<AnimationNode>(animationGraph, iter->tree.get());
    nextAnimation->key = stateKey;

    auto crossFade = std::make_shared<detail::AnimationCrossFadeNode>(currentAnimation, *nextAnimation, transitionDuration, time);

    currentAnimation.node = crossFade;
    time = AnimationTimeInterval::zero();
}

void Animator::Impl::play(u32 stateKey)
{
    POMDOG_ASSERT(animationGraph);
    POMDOG_ASSERT(!animationGraph->states.empty());

    auto iter = findState(animationGraph->states, stateKey);
    POMDOG_ASSERT(iter != std::end(animationGraph->states));

    if (iter == std::end(animationGraph->states)) {
        // Error: Cannot find the state
        return;
    }

    POMDOG_ASSERT(iter->tree);
    currentAnimation.node = std::shared_ptr<AnimationNode>(animationGraph, iter->tree.get());
    currentAnimation.key = stateKey;
    time = AnimationTimeInterval::zero();
}

void Animator::Impl::setFloat(u32 parameterKey, f32 value)
{
    POMDOG_ASSERT(!std::isnan(value));
    POMDOG_ASSERT(animationGraph);
    if (auto index = animationGraph->findParameter(parameterKey); index != std::nullopt) {
        graphWeights.setValue(*index, value);
    }
}

void Animator::Impl::setBool(u32 parameterKey, bool value)
{
    POMDOG_ASSERT(animationGraph);
    if (auto index = animationGraph->findParameter(parameterKey); index != std::nullopt) {
        graphWeights.setValue(*index, value);
    }
}

// MARK: - Animator class

Animator::Animator(
    const std::shared_ptr<Skeleton>& skeleton,
    const std::shared_ptr<skeletal2d::SkeletonPose>& skeletonPose,
    const std::shared_ptr<AnimationGraph>& animationGraph)
    : impl_(std::make_unique<Impl>(skeleton, skeletonPose, animationGraph))
{
}

Animator::~Animator() = default;

void Animator::update(const Duration& frameDuration)
{
    POMDOG_ASSERT(impl_);
    impl_->update(frameDuration);
}

void Animator::crossFade(u32 stateKey, const Duration& transitionDuration)
{
    POMDOG_ASSERT(impl_);
    impl_->crossFade(stateKey, transitionDuration);
}

void Animator::play(u32 stateKey)
{
    POMDOG_ASSERT(impl_);
    impl_->play(stateKey);
}

f32 Animator::getPlaybackRate() const noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->playbackRate;
}

void Animator::setPlaybackRate(f32 playbackRateIn) noexcept
{
    POMDOG_ASSERT(impl_);
    impl_->playbackRate = playbackRateIn;
}

void Animator::setFloat(u32 parameterKey, f32 value)
{
    POMDOG_ASSERT(impl_);
    impl_->setFloat(parameterKey, value);
}

void Animator::setBool(u32 parameterKey, bool value)
{
    POMDOG_ASSERT(impl_);
    impl_->setBool(parameterKey, value);
}

u32 Animator::getCurrentStateKey() const noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->currentAnimation.key;
}

} // namespace pomdog::skeletal2d
