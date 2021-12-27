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
    T compensation = T::zero();

public:
    void operator()(T& sum, const T& num)
    {
        // Kahan summation algorithm
        // NOTE: "Velvel" means "wolf" in Yiddish.
        auto tmp = num - compensation;
        auto velvel = sum + tmp;
        compensation = (velvel - sum) - tmp;
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
    Initialize(int framesPerSecond, const std::shared_ptr<TimeSource>& timeSource) noexcept;

    void Restart();

    /// Increments frame number.
    /// Fires @ref OnTick.
    void Tick() override;

    /// @return Total amount of elapsed time in seconds since the game launch.
    Duration GetTotalGameTime() const noexcept override;

    /// @return Number of current frame in usage.
    /// @note Can't be greater than framesPerSecond in GameClock(int).
    std::int64_t GetFrameNumber() const noexcept override;

    /// @return Duration in seconds of one frame per one second.
    Duration GetFrameDuration() const noexcept override;

    /// @return Approximate quantity of actual frames per second.
    float GetFrameRate() const noexcept override;

    /// @return Total amount of elasped time in seconds since last Tick().
    Duration GetElapsedTime() const noexcept override;

private:
    Duration GetExactLastFrameDuration();

    void AddToFrameHistory(Duration&& exactFrameDuration);

    Duration GetPredictFrameDuration() const;

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
