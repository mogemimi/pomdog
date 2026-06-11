// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class Game;
struct GameHostOptions;
} // namespace pomdog

namespace pomdog {

/// GameSetup is the entry point for game application configuration.
///
/// Game developers implement this class to configure the game host and to
/// create the game. configure() runs before the GameHost is initialized,
/// which is the right place to:
/// - parse command-line arguments,
/// - initialize the virtual file system (VFS),
/// - load configuration files from the VFS,
/// - set GameHost options (graphics backend, window size, subsystem toggles).
///
/// Lifecycle: a GameSetup is not persistent. The Bootstrap destroys it
/// immediately after createGame() returns successfully. Any resource created
/// during configure() that the Game needs, such as a VFS context, must be
/// transferred to the Game in createGame(), for example through the Game's
/// constructor.
///
/// Initialization order:
/// 1. GameSetup::configure(): prepare options, set up the VFS, load configs.
/// 2. The GameHost is initialized with the configured options.
/// 3. GameSetup::createGame(): create the Game; the GameSetup is destroyed.
/// 4. Game::initialize(): game-specific initialization with the GameHost.
/// 5. Main loop: Game::update() and Game::draw() every frame.
///
/// Example:
/// ```cpp
/// class MyApp final : public GameSetup {
///     std::shared_ptr<vfs::FileSystemContext> fs_;
///
///     std::unique_ptr<Error>
///     configure(GameHostOptions& options, std::span<const char* const> args) override
///     {
///         // ... parse CLI args, set up VFS, configure options ...
///         return nullptr;
///     }
///
///     std::unique_ptr<Game>
///     createGame() override
///     {
///         // Transfer fs_ ownership to the Game so it outlives this GameSetup.
///         return std::make_unique<MyGame>(fs_);
///     }
/// };
/// ```
class POMDOG_EXPORT GameSetup {
public:
    GameSetup();

    GameSetup(const GameSetup&) = delete;
    GameSetup& operator=(const GameSetup&) = delete;

    virtual ~GameSetup();

    /// Configures the game host options. Called once, before the GameHost is
    /// created.
    ///
    /// `options` arrives filled with platform-appropriate defaults chosen by
    /// the Bootstrap and is modified in place. `args` holds the command-line
    /// arguments including argv[0]; it may be empty on platforms without a
    /// command line, such as the web.
    ///
    /// Returns an error to abort startup, for example when a required asset
    /// archive is missing or an argument is invalid.
    [[nodiscard]] virtual std::unique_ptr<Error>
    configure(GameHostOptions& options, std::span<const char* const> args) = 0;

    /// Creates the Game instance. Called once, after the GameHost has been
    /// initialized successfully.
    ///
    /// Hand over any data prepared in configure(), such as the VFS context,
    /// to the Game here; the GameSetup is destroyed right after this call
    /// returns. Returning nullptr aborts startup with an error.
    [[nodiscard]] virtual std::unique_ptr<Game>
    createGame() = 0;
};

} // namespace pomdog
