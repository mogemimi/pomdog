// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/chrono/game_clock.h"
#include "pomdog/chrono/time_point.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <deque>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail {
class TimeSource;
} // namespace pomdog::detail

namespace pomdog::detail {

template <typename T>
class CompensatedSumAccumulator final {
private:
    T compensation_ = T::zero();

public:
    void operator()(T& sum, const T& num)
    {
        // Kahan summation algorithm
        // NOTE: "Velvel" means "wolf" in Yiddish.
        const auto tmp = num - compensation_;
        const auto velvel = sum + tmp;
        compensation_ = (velvel - sum) - tmp;
        sum = velvel;
    }
};

/// Application-based time storage.
///
/// Instances of this class are unique.
class POMDOG_EXPORT GameClockImpl final : public GameClock {
public:
    /// Constructs GameClockImpl with initial FPS.
    /// @param framesPerSecond Amount of update / render cycles in one second. Should be greater than 0.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(int framesPerSecond, const std::shared_ptr<TimeSource>& timeSource) noexcept;

    void restart();

    /// Increments frame number.
    /// Fires @ref onTick.
    void tick() override;

    /// @return Total amount of elapsed time in seconds since the game launch.
    [[nodiscard]] Duration
    getTotalGameTime() const noexcept override;

    /// @return Number of current frame in usage.
    /// @note Can't be greater than framesPerSecond in GameClock(int).
    [[nodiscard]] std::int64_t
    getFrameNumber() const noexcept override;

    /// @return Duration in seconds of one frame per one second.
    [[nodiscard]] Duration
    getFrameDuration() const noexcept override;

    /// @return Approximate quantity of actual frames per second.
    [[nodiscard]] float
    getFrameRate() const noexcept override;

    /// @return Total amount of elasped time in seconds since last tick().
    [[nodiscard]] Duration
    getElapsedTime() const noexcept override;

private:
    [[nodiscard]] Duration
    getExactLastFrameDuration();

    void addToFrameHistory(Duration&& exactFrameDuration);

    [[nodiscard]] Duration
    getPredictFrameDuration() const;

private:
    std::shared_ptr<detail::TimeSource> timeSource_;

    TimePoint sourceStartTime_;
    TimePoint sourceLastTime_;

    // FIXME: replace with CircularBuffer<Duration>
    std::deque<Duration> frameDurationHistory_;

    Duration predictedFrameTime_;
    Duration accumulatedCurrentTime_;
    CompensatedSumAccumulator<Duration> accumulator_;
    std::int64_t frameNumber_;
};

} // namespace pomdog::detail
