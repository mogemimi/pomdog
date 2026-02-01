// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/signals/signal.h"

namespace pomdog {

/// Application-based time storage.
///
/// Instances of this class are unique.
class POMDOG_EXPORT GameClock {
public:
    GameClock() noexcept;
    GameClock(const GameClock&) = delete;
    GameClock& operator=(const GameClock&) = delete;

    virtual ~GameClock();

    /// Increments frame number.
    /// Fires @ref onTick.
    virtual void tick() = 0;

    /// @return Total amount of elapsed time in seconds since the game launch.
    [[nodiscard]] virtual Duration
    getTotalGameTime() const noexcept = 0;

    /// @return Number of current frame in usage.
    /// @note Can't be greater than framesPerSecond in GameClock(int).
    [[nodiscard]] virtual i64
    getFrameNumber() const noexcept = 0;

    /// @return Duration in seconds of one frame per one second.
    [[nodiscard]] virtual Duration
    getFrameDuration() const noexcept = 0;

    /// @return Approximate quantity of actual frames per second.
    [[nodiscard]] virtual f32
    getFrameRate() const noexcept = 0;

    /// @return Total amount of elasped time in seconds since last tick().
    [[nodiscard]] virtual Duration
    getElapsedTime() const noexcept = 0;

    /// Signal that fires on every Tick().
    Signal<void(const Duration& frameDuration)> onTick;
};

} // namespace pomdog
