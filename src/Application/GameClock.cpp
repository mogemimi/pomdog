//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Application/GameClock.hpp>
#include "TimeSource.hpp"
#include <Pomdog/Utility/Assert.hpp>
#include <deque>
#include <algorithm>
#include <numeric>
#include <chrono>

namespace Pomdog {
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - GameClock::Impl
#endif
//-----------------------------------------------------------------------
class GameClock::Impl {
public:
	Impl();
	
	void Tick();
	
	void Restart();

	DurationSeconds TotalGameTime() const;
	
	std::uint32_t FrameNumber() const;

	DurationSeconds FrameDuration() const;

	float FrameRate() const;

	DurationSeconds ElapsedTime() const;

private:
	DurationSeconds GetExactLastFrameDuration();
	void AddToFrameHistory(DurationSeconds const& exactFrameDuration);
	DurationSeconds PredictFrameDuration() const;

public:
	Details::TimeSource timeSource;
	TimePointSeconds sourceStartTime;
	TimePointSeconds sourceLastTime;
	std::deque<DurationSeconds> frameDurationHistory;
	
	DurationSeconds predictedFrameTime;
	DurationSeconds accumulatedCurrentTime;
	std::uint32_t frameNumber;
};
//-----------------------------------------------------------------------
GameClock::Impl::Impl()
	: predictedFrameTime(0)
	, accumulatedCurrentTime(0)
	, frameNumber(0)
{
	// frameDefault = (1.0 sec) / (30 fps) = 0.033 sec/frame
	DurationSeconds frameDefault(0.033);
	frameDurationHistory.clear();
	frameDurationHistory.push_back(frameDefault);

	Restart();
}
//-----------------------------------------------------------------------
void GameClock::Impl::Restart()
{
	sourceStartTime = timeSource.Now();
	sourceLastTime = sourceLastTime;
}
//-----------------------------------------------------------------------
DurationSeconds GameClock::Impl::GetExactLastFrameDuration()
{
	auto now = timeSource.Now();
	auto frameDuration = now - sourceLastTime;
	if (0.200 < frameDuration.count())
	{
		frameDuration = frameDurationHistory.back();
	}
	sourceLastTime = now;
	return frameDuration;
}
//-----------------------------------------------------------------------
void GameClock::Impl::AddToFrameHistory(DurationSeconds const& exactFrameDuration)
{
	constexpr std::size_t MaxFrameHistorySize = 20;

	frameDurationHistory.push_back(exactFrameDuration);
	if (frameDurationHistory.size() > MaxFrameHistorySize)
	{
		frameDurationHistory.pop_front();
	}
}
//-----------------------------------------------------------------------
DurationSeconds GameClock::Impl::PredictFrameDuration() const
{
	POMDOG_ASSERT(!frameDurationHistory.empty());

	auto totalFrameTime = std::accumulate(
		std::begin(frameDurationHistory), std::end(frameDurationHistory), DurationSeconds{0});
	return totalFrameTime / frameDurationHistory.size();
}
//-----------------------------------------------------------------------
void GameClock::Impl::Tick()
{
	auto exactLastFrameDuration = GetExactLastFrameDuration();
	AddToFrameHistory(exactLastFrameDuration);
	
	predictedFrameTime = PredictFrameDuration();
	
	accumulatedCurrentTime += predictedFrameTime;
	++frameNumber;
}
//-----------------------------------------------------------------------
DurationSeconds GameClock::Impl::TotalGameTime() const
{
	return accumulatedCurrentTime;
}
//-----------------------------------------------------------------------
std::uint32_t GameClock::Impl::FrameNumber() const
{
	return frameNumber;
}
//-----------------------------------------------------------------------
DurationSeconds GameClock::Impl::FrameDuration() const
{
	return predictedFrameTime;
}
//-----------------------------------------------------------------------
float GameClock::Impl::FrameRate() const
{
	POMDOG_ASSERT(predictedFrameTime.count() != 0);
	return 1 / predictedFrameTime.count();
}
//-----------------------------------------------------------------------
DurationSeconds GameClock::Impl::ElapsedTime() const
{
	return timeSource.Now() - sourceLastTime;
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - GameClock
#endif
//-----------------------------------------------------------------------
GameClock::GameClock()
	: impl(std::make_unique<Impl>())
{}
//-----------------------------------------------------------------------
GameClock::~GameClock() = default;
//-----------------------------------------------------------------------
void GameClock::Tick()
{
	POMDOG_ASSERT(impl);
	impl->Tick();
}
//-----------------------------------------------------------------------
DurationSeconds GameClock::TotalGameTime() const
{
	POMDOG_ASSERT(impl);
	return impl->TotalGameTime();
}
//-----------------------------------------------------------------------
std::uint32_t GameClock::FrameNumber() const
{
	POMDOG_ASSERT(impl);
	return impl->FrameNumber();
}
//-----------------------------------------------------------------------
DurationSeconds GameClock::FrameDuration() const
{
	POMDOG_ASSERT(impl);
	return impl->FrameDuration();
}
//-----------------------------------------------------------------------
float GameClock::FrameRate() const
{
	POMDOG_ASSERT(impl);
	return impl->FrameRate();
}
//-----------------------------------------------------------------------
DurationSeconds GameClock::ElapsedTime() const
{
	POMDOG_ASSERT(impl);
	return impl->ElapsedTime();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
