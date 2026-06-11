// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class GameHost;
} // namespace pomdog

namespace pomdog {

/// Game is the base class for the application's game logic and rendering.
///
/// Game developers implement this class and return an instance from
/// `GameSetup::createGame()`. The GameHost drives the instance: initialize()
/// once at startup, then update() and draw() every frame until the loop
/// exits.
///
/// Instances of this class are unique.
class POMDOG_EXPORT Game {
public:
    /// Creates an empty game.
    Game();

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    virtual ~Game();

    /// Initializes the game. Called by the GameHost once, before the first
    /// update() and draw().
    ///
    /// `gameHost` provides the engine subsystems (window, clock, graphics,
    /// audio, input). Store the handles your game needs here; they stay valid
    /// for the lifetime of the game. Use this method to load assets and
    /// create GPU resources. Command-line parsing and VFS setup belong in
    /// `GameSetup::configure()` instead, which runs before the GameHost
    /// exists.
    ///
    /// Returns an error to abort startup.
    [[nodiscard]] virtual std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost) = 0;

    /// Updates the game logic. Called by the GameHost once per frame, before
    /// draw().
    ///
    /// The timestep is variable: read the elapsed time from the GameClock
    /// (`GameHost::getClock()`), typically `getFrameDuration()`, instead of
    /// assuming a fixed interval. The frame rate follows the V-Sync mode and
    /// the optional frame-rate cap; see `GameHost::setMaxFramesPerSecond()`.
    virtual void
    update() = 0;

    /// Renders a frame. Called by the GameHost once per frame, after
    /// update().
    ///
    /// Keep simulation and state changes in update(): the host may skip
    /// draw() when the frame cannot be presented, for example while the
    /// window is minimized on desktop platforms, while update() continues to
    /// run.
    virtual void
    draw() = 0;
};

} // namespace pomdog
