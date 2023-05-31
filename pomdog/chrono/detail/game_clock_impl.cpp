// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/chrono/detail/game_clock_impl.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/chrono/time_source.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <chrono>
#include <numeric>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

std::unique_ptr<Error>
GameClockImpl::initialize(int framesPerSecond, const std::shared_ptr<TimeSource>& timeSource) noexcept
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
    predictedFrameTime_ = getPredictFrameDuration();

    restart();

    return nullptr;
}

void GameClockImpl::restart()
{
    POMDOG_ASSERT(timeSource_ != nullptr);

    sourceStartTime_ = timeSource_->now();
    sourceLastTime_ = sourceStartTime_;
}

void GameClockImpl::tick()
{
    POMDOG_ASSERT(timeSource_ != nullptr);

    auto exactLastFrameDuration = getExactLastFrameDuration();
    addToFrameHistory(std::move(exactLastFrameDuration));

    predictedFrameTime_ = getPredictFrameDuration();
    POMDOG_ASSERT(predictedFrameTime_.count() > 0);

    accumulator_(accumulatedCurrentTime_, predictedFrameTime_);
    ++frameNumber_;
    POMDOG_ASSERT(frameNumber_ > 0);

    onTick(getFrameDuration());
}

Duration GameClockImpl::getExactLastFrameDuration()
{
    POMDOG_ASSERT(timeSource_ != nullptr);

    const auto now = timeSource_->now();
    auto frameDuration = now - sourceLastTime_;
    if (0.200 < frameDuration.count()) {
        frameDuration = frameDurationHistory_.back();
    }
    sourceLastTime_ = now;
    return frameDuration;
}

void GameClockImpl::addToFrameHistory(Duration&& exactFrameDuration)
{
    constexpr std::size_t MaxFrameHistorySize = 20;

    if (frameDurationHistory_.size() >= MaxFrameHistorySize) {
        frameDurationHistory_.pop_front();
    }
    frameDurationHistory_.push_back(std::move(exactFrameDuration));
}

Duration GameClockImpl::getPredictFrameDuration() const
{
    POMDOG_ASSERT(!frameDurationHistory_.empty());
    POMDOG_ASSERT(frameDurationHistory_.size() > 0);

    auto totalFrameTime = std::accumulate(
        std::begin(frameDurationHistory_),
        std::end(frameDurationHistory_),
        Duration::zero());
    return totalFrameTime / frameDurationHistory_.size();
}

Duration GameClockImpl::getTotalGameTime() const noexcept
{
    return accumulatedCurrentTime_;
}

std::int64_t GameClockImpl::getFrameNumber() const noexcept
{
    return frameNumber_;
}

Duration GameClockImpl::getFrameDuration() const noexcept
{
    return predictedFrameTime_;
}

float GameClockImpl::getFrameRate() const noexcept
{
    POMDOG_ASSERT(!frameDurationHistory_.empty());
    POMDOG_ASSERT(predictedFrameTime_.count() > 0);
    return static_cast<float>(1.0 / predictedFrameTime_.count());
}

Duration GameClockImpl::getElapsedTime() const noexcept
{
    POMDOG_ASSERT(timeSource_ != nullptr);
    return timeSource_->now() - sourceLastTime_;
}

} // namespace pomdog::detail
