// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Application/GameClock.hpp"
#include "TimeSource.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <chrono>
#include <deque>
#include <numeric>

namespace Pomdog {
namespace {

template <typename T>
class CompensatedSumAccumulator {
private:
    T compensation = T::zero();

public:
    void operator()(T & sum, const T& num)
    {
        // Kahan summation algorithm
        // NOTE: "Velvel" means "wolf" in Yiddish.
        auto tmp = num - compensation;
        auto velvel = sum + tmp;
        compensation = (velvel - sum) - tmp;
        sum = velvel;
    }
};

} // unnamed namespace

// MARK: - GameClock::Impl

class GameClock::Impl final {
public:
    explicit Impl(int framesPerSecond = 30);

    void Tick();

    void Restart();

    Duration GetTotalGameTime() const noexcept;

    std::int64_t GetFrameNumber() const noexcept;

    Duration GetFrameDuration() const noexcept;

    float GetFrameRate() const noexcept;

    Duration GetElapsedTime() const noexcept;

private:
    Duration GetExactLastFrameDuration();
    void AddToFrameHistory(const Duration& exactFrameDuration);
    Duration GetPredictFrameDuration() const;

public:
    Detail::TimeSource timeSource;
    TimePoint sourceStartTime;
    TimePoint sourceLastTime;
    std::deque<Duration> frameDurationHistory;

    Duration predictedFrameTime;
    Duration accumulatedCurrentTime;
    CompensatedSumAccumulator<Duration> accumulator;
    std::int64_t frameNumber;
};

GameClock::Impl::Impl(int framesPerSecond)
    : predictedFrameTime(Duration::zero())
    , accumulatedCurrentTime(Duration::zero())
    , frameNumber(0)
{
    POMDOG_ASSERT(framesPerSecond > 0);
    POMDOG_ASSERT(framesPerSecond < 300);
    const Duration frameDefault = Duration(1.0) / framesPerSecond;

    frameDurationHistory.clear();
    frameDurationHistory.push_back(frameDefault);
    predictedFrameTime = GetPredictFrameDuration();

    Restart();
}

void GameClock::Impl::Restart()
{
    sourceStartTime = timeSource.Now();
    sourceLastTime = sourceStartTime;
}

Duration GameClock::Impl::GetExactLastFrameDuration()
{
    auto now = timeSource.Now();
    auto frameDuration = now - sourceLastTime;
    if (0.200 < frameDuration.count()) {
        frameDuration = frameDurationHistory.back();
    }
    sourceLastTime = now;
    return frameDuration;
}

void GameClock::Impl::AddToFrameHistory(const Duration& exactFrameDuration)
{
    constexpr std::size_t MaxFrameHistorySize = 20;

    frameDurationHistory.push_back(exactFrameDuration);
    if (frameDurationHistory.size() > MaxFrameHistorySize) {
        frameDurationHistory.pop_front();
    }
}

Duration GameClock::Impl::GetPredictFrameDuration() const
{
    POMDOG_ASSERT(!frameDurationHistory.empty());
    POMDOG_ASSERT(frameDurationHistory.size() > 0);

    auto totalFrameTime = std::accumulate(
        std::begin(frameDurationHistory),
        std::end(frameDurationHistory),
        Duration::zero());
    return totalFrameTime / frameDurationHistory.size();
}

void GameClock::Impl::Tick()
{
    auto exactLastFrameDuration = GetExactLastFrameDuration();
    AddToFrameHistory(exactLastFrameDuration);

    predictedFrameTime = GetPredictFrameDuration();
    POMDOG_ASSERT(predictedFrameTime.count() > 0);

    accumulator(accumulatedCurrentTime, predictedFrameTime);
    ++frameNumber;
    POMDOG_ASSERT(frameNumber > 0);
}

Duration GameClock::Impl::GetTotalGameTime() const noexcept
{
    return accumulatedCurrentTime;
}

std::int64_t GameClock::Impl::GetFrameNumber() const noexcept
{
    return frameNumber;
}

Duration GameClock::Impl::GetFrameDuration() const noexcept
{
    return predictedFrameTime;
}

float GameClock::Impl::GetFrameRate() const noexcept
{
    POMDOG_ASSERT(!frameDurationHistory.empty());
    POMDOG_ASSERT(predictedFrameTime.count() > 0);
    return static_cast<float>(1.0 / predictedFrameTime.count());
}

Duration GameClock::Impl::GetElapsedTime() const noexcept
{
    return timeSource.Now() - sourceLastTime;
}

// MARK: - GameClock

GameClock::GameClock()
    : impl(std::make_unique<Impl>())
{}

GameClock::GameClock(int framesPerSecond)
    : impl(std::make_unique<Impl>(framesPerSecond))
{
    POMDOG_ASSERT(framesPerSecond > 0);
}

GameClock::~GameClock() = default;

void GameClock::Tick()
{
    POMDOG_ASSERT(impl);
    impl->Tick();

    OnTick(impl->GetFrameDuration());
}

Duration GameClock::GetTotalGameTime() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetTotalGameTime();
}

std::int64_t GameClock::GetFrameNumber() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetFrameNumber();
}

Duration GameClock::GetFrameDuration() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetFrameDuration();
}

float GameClock::GetFrameRate() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetFrameRate();
}

Duration GameClock::GetElapsedTime() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetElapsedTime();
}

} // namespace Pomdog
