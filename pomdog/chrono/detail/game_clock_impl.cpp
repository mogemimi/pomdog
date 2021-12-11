// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/chrono/detail/game_clock_impl.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/chrono/time_source.hpp"
#include "pomdog/utility/assert.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <chrono>
#include <numeric>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

std::unique_ptr<Error>
GameClockImpl::Initialize(int framesPerSecond, const std::shared_ptr<TimeSource>& timeSource) noexcept
{
    POMDOG_ASSERT(framesPerSecond > 0);
    POMDOG_ASSERT(framesPerSecond < 300);
    POMDOG_ASSERT(timeSource != nullptr);
    timeSource_ = timeSource;

    predictedFrameTime_ = Duration::zero();
    accumulatedCurrentTime_ = Duration::zero();
    frameNumber_ = 0;

    Duration frameDefault = Duration(1.0) / framesPerSecond;

    frameDurationHistory_.clear();
    frameDurationHistory_.push_back(std::move(frameDefault));
    predictedFrameTime_ = GetPredictFrameDuration();

    Restart();

    return nullptr;
}

void GameClockImpl::Restart()
{
    POMDOG_ASSERT(timeSource_ != nullptr);

    sourceStartTime_ = timeSource_->Now();
    sourceLastTime_ = sourceStartTime_;
}

void GameClockImpl::Tick()
{
    POMDOG_ASSERT(timeSource_ != nullptr);

    auto exactLastFrameDuration = GetExactLastFrameDuration();
    AddToFrameHistory(std::move(exactLastFrameDuration));

    predictedFrameTime_ = GetPredictFrameDuration();
    POMDOG_ASSERT(predictedFrameTime_.count() > 0);

    accumulator_(accumulatedCurrentTime_, predictedFrameTime_);
    ++frameNumber_;
    POMDOG_ASSERT(frameNumber_ > 0);

    OnTick(GetFrameDuration());
}

Duration GameClockImpl::GetExactLastFrameDuration()
{
    POMDOG_ASSERT(timeSource_ != nullptr);

    const auto now = timeSource_->Now();
    auto frameDuration = now - sourceLastTime_;
    if (0.200 < frameDuration.count()) {
        frameDuration = frameDurationHistory_.back();
    }
    sourceLastTime_ = now;
    return frameDuration;
}

void GameClockImpl::AddToFrameHistory(Duration&& exactFrameDuration)
{
    constexpr std::size_t MaxFrameHistorySize = 20;

    if (frameDurationHistory_.size() >= MaxFrameHistorySize) {
        frameDurationHistory_.pop_front();
    }
    frameDurationHistory_.push_back(std::move(exactFrameDuration));
}

Duration GameClockImpl::GetPredictFrameDuration() const
{
    POMDOG_ASSERT(!frameDurationHistory_.empty());
    POMDOG_ASSERT(frameDurationHistory_.size() > 0);

    auto totalFrameTime = std::accumulate(
        std::begin(frameDurationHistory_),
        std::end(frameDurationHistory_),
        Duration::zero());
    return totalFrameTime / frameDurationHistory_.size();
}

Duration GameClockImpl::GetTotalGameTime() const noexcept
{
    return accumulatedCurrentTime_;
}

std::int64_t GameClockImpl::GetFrameNumber() const noexcept
{
    return frameNumber_;
}

Duration GameClockImpl::GetFrameDuration() const noexcept
{
    return predictedFrameTime_;
}

float GameClockImpl::GetFrameRate() const noexcept
{
    POMDOG_ASSERT(!frameDurationHistory_.empty());
    POMDOG_ASSERT(predictedFrameTime_.count() > 0);
    return static_cast<float>(1.0 / predictedFrameTime_.count());
}

Duration GameClockImpl::GetElapsedTime() const noexcept
{
    POMDOG_ASSERT(timeSource_ != nullptr);
    return timeSource_->Now() - sourceLastTime_;
}

} // namespace pomdog::detail
