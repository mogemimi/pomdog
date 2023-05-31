// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/signals/signal.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class GameClock;

/// A facility to execute tasks in the future.
/// Tasks may be scheduled for one time execution, or for repeated execution
/// at regular intervals.
///
/// You can set the task by connecting it with @ref Elapsed signal.
///
/// Works by utilizing GameClock::Tick().
///
/// Instances of this class are unique.
class POMDOG_EXPORT Timer {
private:
    ScopedConnection connection_;
    std::optional<Duration> interval_;
    Duration totalTime_;
    double scale_;
    bool enabled_;
    bool singleShot_;

public:
    /// Creates the Timer which is connected to given GameClock.
    /// The timer starts instantly after construction in repeated execution mode
    /// with zero duration.
    /// @param clock Working GameClock instance.
    explicit Timer(GameClock& clock);

    /// Creates the Timer which is connected to given GameClock pointer.
    /// @see Timer(GameClock&).
    /// @param clock Pointer to working GameClock instance.
    explicit Timer(const std::shared_ptr<GameClock>& clock);

    Timer() = delete;
    Timer(const Timer&) = delete;
    Timer(Timer&&) = default;
    Timer& operator=(const Timer&) = delete;
    Timer& operator=(Timer&&) = default;

    ~Timer();

    /// Enables the timer.
    void start();

    /// Disables the timer with saving total elapsed time.
    void stop();

    /// Sets total elapsed time to zero.
    void reset();

    /// @return If timer is running.
    [[nodiscard]] bool
    isEnabled() const;

    /// @return Total elapsed time while timer was running.
    [[nodiscard]] Duration
    getTotalTime() const;

    /// @return If timer is executing the task only once.
    [[nodiscard]] bool
    isSingleShot() const;

    /// Sets timer task repetition.
    /// @param singleShot True if task must be executed only once, false otherwise.
    void setSingleShot(bool singleShot);

    /// @return Time scale of the timer.
    [[nodiscard]] double
    getScale() const;

    /// Sets the scale which affects duration of every frame that is
    /// summed up in total elapsed time.
    /// @param scale New scale value.
    void setScale(double scale);

    /// @return Interval between task execution, if was given.
    [[nodiscard]] std::optional<Duration>
    getInterval() const;

    /// Sets interval between task execution.
    /// @param interval New interval value.
    void setInterval(const Duration& interval);

    /// Resets interval between task execution to undefined value.
    void setInterval();

    /// Signal that fires when timer's elapsed time is over the duration.
    /// Can be fired multiple times, if the timer is set to re-execution.
    Signal<void()> elapsed;
};

} // namespace pomdog
