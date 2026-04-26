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

/// Entry point for game application configuration.
///
/// Game developers implement this class to configure the game host and create the game.
/// The configure() method is called BEFORE GameHost initialization, allowing the game
/// developer to:
/// - Parse command-line arguments
/// - Initialize the virtual file system (VFS)
/// - Load configuration files from VFS
/// - Set GameHost options (graphics backend, window size, subsystem toggles, etc.)
///
/// ## Lifecycle
/// This class is NOT persistent throughout the application lifetime. It is destroyed by
/// the Bootstrap immediately after createGame() returns successfully. Any resources
/// (VFS contexts, configuration data, etc.) created during configure() that the Game
/// needs must be transferred to the Game or GameHost via createGame() — for example,
/// by passing them through the Game's constructor.
///
/// ## Initialization Order
/// 1. GameSetup::configure() — prepare options, VFS, load configs
/// 2. GameHost is initialized with the options
/// 3. GameSetup::createGame() — create the Game instance, then GameSetup is destroyed
/// 4. Game::initialize() — game-specific initialization with access to GameHost
/// 5. Main loop: Game::update() / Game::draw()
///
/// ## Example
/// ```cpp
/// class MyApp final : public GameSetup {
///     std::shared_ptr<vfs::FileSystemContext> fs_;
///
///     std::unique_ptr<Error>
///     configure(GameHostOptions& options, std::span<const char* const> args) override
///     {
///         CLIParser cli;
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

    /// Configure the game host options before GameHost initialization.
    ///
    /// Called once before the GameHost is created. Use this to:
    /// - Parse command-line arguments using CLIParser
    /// - Initialize the VFS and load configuration files
    /// - Set graphics backend, window size, and other host options
    /// - Load game controller database from VFS for gamepad support
    ///
    /// @param options The options to configure. Modify in-place. Contains
    ///        platform-appropriate defaults set by the Bootstrap.
    /// @param args Command-line arguments including argv[0] (program name).
    ///        May be empty (e.g., on Emscripten where there are no CLI args).
    [[nodiscard]] virtual std::unique_ptr<Error>
    configure(GameHostOptions& options, std::span<const char* const> args) = 0;

    /// Create the Game instance after GameHost is initialized.
    ///
    /// Called once after GameHost is successfully initialized.
    /// The game developer can pass any data prepared in configure()
    /// (such as VFS context) to the Game through its constructor.
    [[nodiscard]] virtual std::unique_ptr<Game>
    createGame() = 0;
};

} // namespace pomdog
