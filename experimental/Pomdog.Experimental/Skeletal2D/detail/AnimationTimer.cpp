//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "AnimationTimer.hpp"

namespace Pomdog {
namespace Details {
namespace Skeletal2D {
//-----------------------------------------------------------------------
AnimationTimer::AnimationTimer()
	: time(AnimationTimeInterval::zero())
	, isPlaying(true)
{
}
//-----------------------------------------------------------------------
void AnimationTimer::Update(AnimationTimeInterval const& timeIn)
{
	if (isPlaying)
	{
		time += timeIn;
	}
}
//-----------------------------------------------------------------------
void AnimationTimer::Reset()
{
	time = AnimationTimeInterval::zero();
}
//-----------------------------------------------------------------------
void AnimationTimer::Pause()
{
	isPlaying = false;
}
//-----------------------------------------------------------------------
void AnimationTimer::Resume()
{
	isPlaying = true;
}
//-----------------------------------------------------------------------
AnimationTimeInterval AnimationTimer::Time() const
{
	return time;
}
//-----------------------------------------------------------------------
bool AnimationTimer::IsPlaying() const
{
	return isPlaying;
}
//-----------------------------------------------------------------------
}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog
