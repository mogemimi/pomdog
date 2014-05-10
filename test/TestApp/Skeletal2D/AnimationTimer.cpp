//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "AnimationTimer.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
AnimationTimer::AnimationTimer()
	: isPause(false)
{
}
//-----------------------------------------------------------------------
void AnimationTimer::Update(DurationSeconds const& timeIn)
{
	if (!isPause)
	{
		time += timeIn;
	}
}
//-----------------------------------------------------------------------
void AnimationTimer::Reset()
{
	time = DurationSeconds(0);
}
//-----------------------------------------------------------------------
void AnimationTimer::Pause()
{
	isPause = true;
}
//-----------------------------------------------------------------------
void AnimationTimer::Continue()
{
	isPause = false;
}
//-----------------------------------------------------------------------
DurationSeconds AnimationTimer::Time() const
{
	return time;
}
//-----------------------------------------------------------------------
bool AnimationTimer::IsPause() const
{
	return isPause;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
