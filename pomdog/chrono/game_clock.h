// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/signals/signal.h"

namespace pomdog {

/// GameClock is an interface for a game clock.
/// It is used to store time information such as total game time, frame duration, and frame rate.
class POMDOG_EXPORT GameClock {
public:
    GameClock() noexcept;
    GameClock(const GameClock&) = delete;
    GameClock& operator=(const GameClock&) = delete;

    virtual ~GameClock();

    /// Increments frame number and updates time information.
    virtual void
    tick() = 0;

    /// Return total amount of elapsed time in seconds since the creation of this instance.
    /// This value is updated at the beginning of each frame. It returns the same value within the same frame.
    [[nodiscard]] virtual Duration
    getTotalGameTime() const noexcept = 0;

    /// Return index of the current frame.
    /// This value is updated at the beginning of each frame. It returns the same value within the same frame.
    [[nodiscard]] virtual i64
    getFrameNumber() const noexcept = 0;

    /// Return duration of the current frame in seconds.
    /// This value is updated at the beginning of each frame. It returns the same value within the same frame.
    [[nodiscard]] virtual Duration
    getFrameDuration() const noexcept = 0;

    /// Return frame rate in frames per second.
    /// This value is updated at the beginning of each frame. It returns the same value within the same frame.
    [[nodiscard]] virtual f32
    getFrameRate() const noexcept = 0;

    /// Return total amount of elapsed time in seconds since the last Tick().
    /// This value represents the elapsed time within the current frame and is reset every time Tick() is called.
    /// It is used to get the elapsed time since the start of the frame.
    /// It is not guaranteed to return the same value if called multiple times within the same frame.
    [[nodiscard]] virtual Duration
    getElapsedTime() const noexcept = 0;

    /// Signal that fires on every Tick().
    Signal<void(const Duration& frameDuration)> onTick;
};

} // namespace pomdog
