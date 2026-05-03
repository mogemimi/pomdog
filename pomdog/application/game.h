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

/// Core application class that describes game logic and rendering.
///
/// Instances of this class are unique.
class POMDOG_EXPORT Game {
public:
    /// Constructs empty Game.
    Game();

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    virtual ~Game();

    /// Initialization phase of the game.
    ///
    /// Called by GameHost once before any Update() or Draw().
    /// Command-line argument parsing and VFS setup should be done in
    /// GameSetup::configure(), not here.
    /// @param gameHost The game host that provides system services.
    [[nodiscard]] virtual std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost) = 0;

    /// Logic update phase of the game.
    ///
    /// Called by GameHost every frame before Draw().
    ///
    /// Logic updates are frame-independent. More specifically, they are guaranteed
    /// to be called once in a *presentation interval*, which is set in a Bootstrap.
    virtual void
    update() = 0;

    /// Rendering phase of the game.
    ///
    /// Called by GameHost after every Update().
    ///
    /// @note Do all computations in Update()
    virtual void
    draw() = 0;
};

} // namespace pomdog
