// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/duration.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/signals/signal.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Application-based time storage.
///
/// Instances of this class are unique.
class POMDOG_EXPORT GameClock final {
public:
    /// Constructs GameClock with default value of FPS.
    /// Default value is 30.
    /// @see GameClock(int)
    GameClock();

    /// Constructs GameClock with initial FPS.
    /// @param framesPerSecond Amount of update / render cycles in one second.
    /// Should be greater than 0.
    explicit GameClock(int framesPerSecond);

    GameClock(const GameClock&) = delete;
    GameClock(GameClock&&) = default;

    GameClock& operator=(const GameClock&) = delete;
    GameClock& operator=(GameClock&&) = default;

    ~GameClock();

    /// Increments frame number.
    /// Fires @ref OnTick.
    void Tick();

    /// @return Total amount of elapsed time in seconds since the game launch.
    Duration GetTotalGameTime() const noexcept;

    /// @return Number of current frame in usage.
    /// @note Can't be greater than framesPerSecond in GameClock(int).
    std::int64_t GetFrameNumber() const noexcept;

    /// @return Duration in seconds of one frame per one second.
    Duration GetFrameDuration() const noexcept;

    /// @return Approximate quantity of actual frames per second.
    float GetFrameRate() const noexcept;

    /// @return Total amount of elasped time in seconds since last Tick().
    Duration GetElapsedTime() const noexcept;

    /// Signal that fires on every Tick().
    Signal<void(const Duration& frameDuration)> OnTick;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace pomdog
