// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/animation_state.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/animation_clip.h"
#include "pomdog/utility/assert.h"

namespace pomdog::skeletal2d {

AnimationState::AnimationState(const std::shared_ptr<AnimationClip>& animationClipIn)
    : AnimationState(animationClipIn, 1, false)
{
}

AnimationState::AnimationState(const std::shared_ptr<AnimationClip>& animationClipIn, f32 playbackRateIn, bool loopIn)
    : clip_(animationClipIn)
    , playbackRate_(playbackRateIn)
    , enabled_(true)
    , loop_(loopIn)
{
}

void AnimationState::update(const AnimationTimeInterval& frameDuration)
{
    time_ = time_ + frameDuration * playbackRate_;
    if (time_ < AnimationTimeInterval::zero()) {
        time_ = getLength();
    }
    else if (time_ > getLength()) {
        POMDOG_ASSERT(loop_);
        time_ = AnimationTimeInterval::zero();
    }
}

const std::shared_ptr<AnimationClip>& AnimationState::getClip() const
{
    return clip_;
}

AnimationTimeInterval AnimationState::getTime() const noexcept
{
    return time_;
}

void AnimationState::setTime(const AnimationTimeInterval& timeIn) noexcept
{
    POMDOG_ASSERT(timeIn.count() >= 0);
    this->time_ = timeIn;
}

AnimationTimeInterval AnimationState::getLength() const
{
    return clip_->getLength();
}

f32 AnimationState::getPlaybackRate() const noexcept
{
    return playbackRate_;
}

void AnimationState::setPlaybackRate(f32 playbackRateIn) noexcept
{
    this->playbackRate_ = playbackRateIn;
}

bool AnimationState::isEnabled() const noexcept
{
    return enabled_;
}

void AnimationState::setEnabled(bool enabledIn) noexcept
{
    this->enabled_ = enabledIn;
}

bool AnimationState::isLoop() const noexcept
{
    return loop_;
}

void AnimationState::setLoop(bool loopIn) noexcept
{
    this->loop_ = loopIn;
}

} // namespace pomdog::skeletal2d
