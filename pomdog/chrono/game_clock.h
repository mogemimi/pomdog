// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/signals/signal.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
    /// Fires @ref OnTick.
    virtual void Tick() = 0;

    /// @return Total amount of elapsed time in seconds since the game launch.
    [[nodiscard]] virtual Duration
    GetTotalGameTime() const noexcept = 0;

    /// @return Number of current frame in usage.
    /// @note Can't be greater than framesPerSecond in GameClock(int).
    [[nodiscard]] virtual std::int64_t
    GetFrameNumber() const noexcept = 0;

    /// @return Duration in seconds of one frame per one second.
    [[nodiscard]] virtual Duration
    GetFrameDuration() const noexcept = 0;

    /// @return Approximate quantity of actual frames per second.
    [[nodiscard]] virtual float
    GetFrameRate() const noexcept = 0;

    /// @return Total amount of elasped time in seconds since last Tick().
    [[nodiscard]] virtual Duration
    GetElapsedTime() const noexcept = 0;

    /// Signal that fires on every Tick().
    Signal<void(const Duration& frameDuration)> OnTick;
};

} // namespace pomdog
