//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "AnimationState.hpp"
#include "AnimationClip.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
AnimationState::AnimationState(std::shared_ptr<AnimationClip> const& animationClipIn)
	: AnimationState(animationClipIn, 1, false)
{}
//-----------------------------------------------------------------------
AnimationState::AnimationState(std::shared_ptr<AnimationClip> const& animationClipIn, float playbackRateIn, bool loopIn)
	: clip(animationClipIn)
	, playbackRate(playbackRateIn)
	, enabled(true)
	, loop(loopIn)
{
}
//-----------------------------------------------------------------------
void AnimationState::Update(AnimationTimeInterval const& frameDuration)
{
	time = time + frameDuration * playbackRate;
	if (time < AnimationTimeInterval::zero()) {
		time = Length();
	}
	else if (time > Length()) {
		POMDOG_ASSERT(loop);
		time = AnimationTimeInterval::zero();
	}
}
//-----------------------------------------------------------------------
std::shared_ptr<AnimationClip> const& AnimationState::Clip() const
{
	return clip;
}
//-----------------------------------------------------------------------
AnimationTimeInterval AnimationState::Time() const
{
	return time;
}
//-----------------------------------------------------------------------
void AnimationState::Time(AnimationTimeInterval const& timeIn)
{
	POMDOG_ASSERT(timeIn.count() >= 0);
	this->time = timeIn;
}
//-----------------------------------------------------------------------
AnimationTimeInterval AnimationState::Length() const
{
	return clip->Length();
}
//-----------------------------------------------------------------------
float AnimationState::PlaybackRate() const
{
	return playbackRate;
}
//-----------------------------------------------------------------------
void AnimationState::PlaybackRate(float playbackRateIn)
{
	this->playbackRate = playbackRateIn;
}
//-----------------------------------------------------------------------
bool AnimationState::Enabled() const
{
	return enabled;
}
//-----------------------------------------------------------------------
void AnimationState::Enabled(bool enabledIn)
{
	this->enabled = enabledIn;
}
//-----------------------------------------------------------------------
bool AnimationState::Loop() const
{
	return loop;
}
//-----------------------------------------------------------------------
void AnimationState::Loop(bool loopIn)
{
	this->loop = loopIn;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
