//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Pomdog/Application/Timer.hpp"
#include "Pomdog/Application/GameClock.hpp"
#include <utility>

namespace Pomdog {
//-----------------------------------------------------------------------
Timer::Timer(GameClock & clock)
	: totalTime(DurationSeconds::zero())
	, frameDuration(DurationSeconds::zero())
	, scale(1.0f)
	, enabled(true)
{
	auto onTick = [this](DurationSeconds const& frameDurationIn) {
		if (!enabled) {
			return;
		}
		this->frameDuration = (frameDurationIn * this->scale);
		this->totalTime += (frameDuration * this->scale);
	};
	connection = clock.OnTick.Connect(std::move(onTick));
}
//-----------------------------------------------------------------------
Timer::~Timer() = default;
//-----------------------------------------------------------------------
void Timer::Start()
{
	this->enabled = true;
}
//-----------------------------------------------------------------------
void Timer::Stop()
{
	this->enabled = false;
}
//-----------------------------------------------------------------------
void Timer::Reset()
{
	totalTime = DurationSeconds::zero();
}
//-----------------------------------------------------------------------
bool Timer::Enabled() const
{
	return this->enabled;
}
//-----------------------------------------------------------------------
DurationSeconds Timer::TotalTime() const
{
	return this->totalTime;
}
//-----------------------------------------------------------------------
DurationSeconds Timer::FrameDuration() const
{
	return this->frameDuration;
}
//-----------------------------------------------------------------------
void Timer::Scale(float scaleIn)
{
	this->scale = scaleIn;
}
//-----------------------------------------------------------------------
float Timer::Scale() const
{
	return this->scale;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
