//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "Clock.hpp"
#include <deque>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <Pomdog/Utility/Assert.hpp>
#include "TimeSource.hpp"

namespace Pomdog {
namespace Details {
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Clock::Impl
#endif
//-----------------------------------------------------------------------
class Clock::Impl
{
public:
	Impl();
	
	void Tick();
	
	void Restart();
	
	TimePointSeconds Now() const;
	
	DurationSeconds TimeSinceEpoch() const;
	
	std::uint32_t FrameNumber() const;

	DurationSeconds FrameDuration() const;

	double FrameRate() const;

	DurationSeconds ElapsedTime() const;

private:
	DurationSeconds GetExactLastFrameDuration();
	void AddToFrameHistory(DurationSeconds const& exactFrameDuration);
	DurationSeconds PredictFrameDuration() const;

public:
	TimeSource timeSource;
	TimePointSeconds sourceStartTime;
	TimePointSeconds sourceLastTime;
	std::deque<DurationSeconds> frameDurationHistory;
	
	DurationSeconds predictedFrameTime;
	DurationSeconds accumulatedCurrentTime;
	std::uint32_t frameNumber;
};
//-----------------------------------------------------------------------
Clock::Impl::Impl()
	: predictedFrameTime(0)
	, accumulatedCurrentTime(0)
	, frameNumber(0)
{
	// frameDefault = (1.0 sec) / (30 fps) = 0.033 sec/frame
	DurationSeconds frameDefault(0.033);
	frameDurationHistory.clear();
	frameDurationHistory.push_back(frameDefault);

	this->Restart();
}
//-----------------------------------------------------------------------
void Clock::Impl::Restart()
{
	sourceStartTime = timeSource.Now();
	sourceLastTime = sourceLastTime;
}
//-----------------------------------------------------------------------
DurationSeconds Clock::Impl::GetExactLastFrameDuration()
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
void Clock::Impl::AddToFrameHistory(DurationSeconds const& exactFrameDuration)
{
	constexpr std::size_t MaxFrameHistorySize = 20;

	frameDurationHistory.push_back(exactFrameDuration);
	if (frameDurationHistory.size() > MaxFrameHistorySize)
	{
		frameDurationHistory.pop_front();
	}
}
//-----------------------------------------------------------------------
DurationSeconds Clock::Impl::PredictFrameDuration() const
{
	POMDOG_ASSERT(!frameDurationHistory.empty());

	auto totalFrameTime = std::accumulate(
		std::begin(frameDurationHistory), std::end(frameDurationHistory), DurationSeconds{0});
	return totalFrameTime / frameDurationHistory.size();
}
//-----------------------------------------------------------------------
void Clock::Impl::Tick()
{
	auto exactLastFrameDuration = GetExactLastFrameDuration();
	AddToFrameHistory(exactLastFrameDuration);
	
	predictedFrameTime = PredictFrameDuration();
	
	accumulatedCurrentTime += predictedFrameTime;
	++frameNumber;
}
//-----------------------------------------------------------------------
TimePointSeconds Clock::Impl::Now() const
{
	return TimePointSeconds{} + accumulatedCurrentTime;
}
//-----------------------------------------------------------------------
DurationSeconds Clock::Impl::TimeSinceEpoch() const
{
	return accumulatedCurrentTime;
}
//-----------------------------------------------------------------------
std::uint32_t Clock::Impl::FrameNumber() const
{
	return frameNumber;
}
//-----------------------------------------------------------------------
DurationSeconds Clock::Impl::FrameDuration() const
{
	return predictedFrameTime;
}
//-----------------------------------------------------------------------
double Clock::Impl::FrameRate() const
{
	POMDOG_ASSERT(predictedFrameTime.count() != 0);
	return 1.0 / predictedFrameTime.count();
}
//-----------------------------------------------------------------------
DurationSeconds Clock::Impl::ElapsedTime() const
{
	return timeSource.Now() - sourceLastTime;
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Clock
#endif
//-----------------------------------------------------------------------
Clock::Clock()
	: impl(new Impl)
{}
//-----------------------------------------------------------------------
Clock::~Clock() = default;
//-----------------------------------------------------------------------
void Clock::Tick()
{
	POMDOG_ASSERT(impl);
	impl->Tick();
}
//-----------------------------------------------------------------------
void Clock::Restart()
{
	POMDOG_ASSERT(impl);
	impl->Restart();
}
//-----------------------------------------------------------------------
TimePointSeconds Clock::Now() const
{
	POMDOG_ASSERT(impl);
	return impl->Now();
}
//-----------------------------------------------------------------------
DurationSeconds Clock::TimeSinceEpoch() const
{
	POMDOG_ASSERT(impl);
	return impl->TimeSinceEpoch();
}
//-----------------------------------------------------------------------
std::uint32_t Clock::FrameNumber() const
{
	POMDOG_ASSERT(impl);
	return impl->FrameNumber();
}
//-----------------------------------------------------------------------
DurationSeconds Clock::FrameDuration() const
{
	POMDOG_ASSERT(impl);
	return impl->FrameDuration();
}
//-----------------------------------------------------------------------
double Clock::FrameRate() const
{
	POMDOG_ASSERT(impl);
	return impl->FrameRate();
}
//-----------------------------------------------------------------------
DurationSeconds Clock::ElapsedTime() const
{
	POMDOG_ASSERT(impl);
	return impl->ElapsedTime();
}
//-----------------------------------------------------------------------
}// namespace Details
}// namespace Pomdog
