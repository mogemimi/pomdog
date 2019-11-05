// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Skeletal2D/AnimationState.hpp"
#include "Pomdog/Experimental/Skeletal2D/AnimationClip.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog::Skeletal2D {

AnimationState::AnimationState(const std::shared_ptr<AnimationClip>& animationClipIn)
    : AnimationState(animationClipIn, 1, false)
{
}

AnimationState::AnimationState(const std::shared_ptr<AnimationClip>& animationClipIn, float playbackRateIn, bool loopIn)
    : clip(animationClipIn)
    , playbackRate(playbackRateIn)
    , enabled(true)
    , loop(loopIn)
{
}

void AnimationState::Update(const AnimationTimeInterval& frameDuration)
{
    time = time + frameDuration * playbackRate;
    if (time < AnimationTimeInterval::zero()) {
        time = GetLength();
    }
    else if (time > GetLength()) {
        POMDOG_ASSERT(loop);
        time = AnimationTimeInterval::zero();
    }
}

const std::shared_ptr<AnimationClip>& AnimationState::Clip() const
{
    return clip;
}

AnimationTimeInterval AnimationState::GetTime() const noexcept
{
    return time;
}

void AnimationState::SetTime(const AnimationTimeInterval& timeIn) noexcept
{
    POMDOG_ASSERT(timeIn.count() >= 0);
    this->time = timeIn;
}

AnimationTimeInterval AnimationState::GetLength() const
{
    return clip->GetLength();
}

float AnimationState::GetPlaybackRate() const noexcept
{
    return playbackRate;
}

void AnimationState::SetPlaybackRate(float playbackRateIn) noexcept
{
    this->playbackRate = playbackRateIn;
}

bool AnimationState::IsEnabled() const noexcept
{
    return enabled;
}

void AnimationState::SetEnabled(bool enabledIn) noexcept
{
    this->enabled = enabledIn;
}

bool AnimationState::IsLoop() const noexcept
{
    return loop;
}

void AnimationState::SetLoop(bool loopIn) noexcept
{
    this->loop = loopIn;
}

} // namespace Pomdog::Skeletal2D
