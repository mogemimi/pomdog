// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/chrono/game_clock.h"
#include "pomdog/chrono/time_point.h"
#include "pomdog/utility/circular_buffer.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
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
    GameClockImpl() noexcept;
    GameClockImpl(const GameClockImpl&) = delete;
    GameClockImpl& operator=(const GameClockImpl&) = delete;

    /// Initializes the clock with the given time source and starts measuring
    /// time from "now".
    ///
    /// The frame-duration predictor starts from a nominal 60 fps estimate, so
    /// getFrameDuration() and getFrameRate() return a sane value before the
    /// first tick(); the estimate converges to the measured frame times within
    /// the first few ticks.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<TimeSource>& timeSource) noexcept;

    /// Resets the reference point for elapsed-time measurement to "now"
    /// without resetting the total game time or the frame number.
    void restart();

    /// Increments the frame number, updates the predicted frame duration from
    /// the measured frame times, and fires onTick.
    void tick() override;

    /// Returns the total amount of elapsed time in seconds since the game launch.
    [[nodiscard]] Duration
    getTotalGameTime() const noexcept override;

    /// Returns the index of the current frame, starting at 0 and incremented
    /// by every tick().
    [[nodiscard]] i64
    getFrameNumber() const noexcept override;

    /// Returns the predicted duration of the current frame in seconds.
    [[nodiscard]] Duration
    getFrameDuration() const noexcept override;

    /// Returns the approximate frame rate in frames per second.
    [[nodiscard]] f32
    getFrameRate() const noexcept override;

    /// Returns the amount of elapsed time in seconds since the last tick().
    [[nodiscard]] Duration
    getElapsedTime() const noexcept override;

private:
    [[nodiscard]] Duration
    getExactLastFrameDuration();

    void addToFrameHistory(Duration&& exactFrameDuration);

    [[nodiscard]] Duration
    getPredictFrameDuration() const;

private:
    std::shared_ptr<detail::TimeSource> timeSource_ = {};

    TimePoint sourceStartTime_ = {};
    TimePoint sourceLastTime_ = {};

    CircularBuffer<Duration> frameDurationHistory_ = {};

    Duration predictedFrameTime_ = {};
    Duration accumulatedCurrentTime_ = {};
    CompensatedSumAccumulator<Duration> accumulator_ = {};
    i64 frameNumber_ = 0;
};

} // namespace pomdog::detail
