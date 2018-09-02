// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include <memory>
#include <optional>

namespace Pomdog {

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
    void Start();

    /// Disables the timer with saving total elapsed time.
    void Stop();

    /// Sets total elapsed time to zero.
    void Reset();

    /// @return If timer is running.
    bool IsEnabled() const;

    /// @return Total elapsed time while timer was running.
    Duration GetTotalTime() const;

    /// @return If timer is executing the task only once.
    bool IsSingleShot() const;

    /// Sets timer task repetition.
    /// @param isSingleShot True if task must be executed only once, false otherwise.
    void SetSingleShot(bool isSingleShot);

    /// @return Time scale of the timer.
    double GetScale() const;

    /// Sets the scale which affects duration of every frame that is
    /// summed up in total elapsed time.
    /// @param scale New scale value.
    void SetScale(double scale);

    /// @return Interval between task execution, if was given.
    std::optional<Duration> GetInterval() const;

    /// Sets interval between task execution.
    /// @param interval New interval value.
    void SetInterval(const Duration& interval);

    /// Resets interval between task execution to undefined value.
    void SetInterval();

    /// Signal that fires when timer's elapsed time is over the duration.
    /// Can be fired multiple times, if the timer is set to re-execution.
    Signal<void()> Elapsed;

private:
    ScopedConnection connection;
    std::optional<Duration> interval;
    Duration totalTime;
    double scale;
    bool enabled;
    bool isSingleShot;
};

} // namespace Pomdog
