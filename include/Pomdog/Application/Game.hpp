// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {
class Error;
} // namespace Pomdog

namespace Pomdog {

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
    [[nodiscard]] virtual std::unique_ptr<Error> Initialize() = 0;

    /// Logic update phase of the game.
    ///
    /// Called by GameHost every frame before Update().
    ///
    /// Logic updates are frame-independent. More specifically, they are guarnteed
    /// to be called once in a *presentation interval*, which is set in a Bootstrap.
    /// @see X11::Bootstrap::SetPresentationInterval,
    ///      Win32::Bootstrap::SetPresentationInterval
    /// @note In Cocoa, *presentation interval* is always 60
    virtual void Update() = 0;

    /// Rendering phase of the game.
    ///
    /// Called by GameHost after every Update().
    ///
    /// @note Do all computations in Update()
    virtual void Draw() = 0;
};

} // namespace Pomdog
