//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Pomdog/Application/GameClock.hpp"
#include "TimeSource.hpp"
#include "Pomdog/Utility/Assert.hpp"
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
class GameClock::Impl final {
public:
	Impl();

	void Tick();

	void Restart();

	Duration TotalGameTime() const;

	std::uint32_t FrameNumber() const;

	Duration FrameDuration() const;

	float FrameRate() const;

	Duration ElapsedTime() const;

private:
	Duration GetExactLastFrameDuration();
	void AddToFrameHistory(Duration const& exactFrameDuration);
	Duration PredictFrameDuration() const;

public:
	Detail::TimeSource timeSource;
	TimePoint sourceStartTime;
	TimePoint sourceLastTime;
	std::deque<Duration> frameDurationHistory;

	Duration predictedFrameTime;
	Duration accumulatedCurrentTime;
	std::uint32_t frameNumber;
};
//-----------------------------------------------------------------------
GameClock::Impl::Impl()
	: predictedFrameTime(0)
	, accumulatedCurrentTime(0)
	, frameNumber(0)
{
	constexpr int framePerSecond = 30;
	const Duration frameDefault = Duration(1.0) / framePerSecond;

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
Duration GameClock::Impl::GetExactLastFrameDuration()
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
void GameClock::Impl::AddToFrameHistory(Duration const& exactFrameDuration)
{
	constexpr std::size_t MaxFrameHistorySize = 20;

	frameDurationHistory.push_back(exactFrameDuration);
	if (frameDurationHistory.size() > MaxFrameHistorySize)
	{
		frameDurationHistory.pop_front();
	}
}
//-----------------------------------------------------------------------
Duration GameClock::Impl::PredictFrameDuration() const
{
	POMDOG_ASSERT(!frameDurationHistory.empty());

	auto totalFrameTime = std::accumulate(
		std::begin(frameDurationHistory), std::end(frameDurationHistory), Duration::zero());
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
Duration GameClock::Impl::TotalGameTime() const
{
	return accumulatedCurrentTime;
}
//-----------------------------------------------------------------------
std::uint32_t GameClock::Impl::FrameNumber() const
{
	return frameNumber;
}
//-----------------------------------------------------------------------
Duration GameClock::Impl::FrameDuration() const
{
	return predictedFrameTime;
}
//-----------------------------------------------------------------------
float GameClock::Impl::FrameRate() const
{
	POMDOG_ASSERT(predictedFrameTime.count() != 0);
	return 1 / static_cast<float>(predictedFrameTime.count());
}
//-----------------------------------------------------------------------
Duration GameClock::Impl::ElapsedTime() const
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

	OnTick(impl->FrameDuration());
}
//-----------------------------------------------------------------------
Duration GameClock::TotalGameTime() const
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
Duration GameClock::FrameDuration() const
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
Duration GameClock::ElapsedTime() const
{
	POMDOG_ASSERT(impl);
	return impl->ElapsedTime();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
