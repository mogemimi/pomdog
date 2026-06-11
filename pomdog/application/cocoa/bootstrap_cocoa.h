// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/graphics_backend.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>

#import <AppKit/NSWindow.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

@class PomdogOpenGLView;
@class PomdogMetalViewController;

namespace pomdog {
class Error;
class Game;
class GameSetup;
class GameHost;
} // namespace pomdog

namespace pomdog::detail::cocoa {
class GameHostCocoa;
class GameHostMetal;
} // namespace pomdog::detail::cocoa

namespace pomdog::cocoa {

/// Bootstrap is the macOS entry point of a Pomdog application.
///
/// Call it from the application delegate: set the native window, then call
/// run() with a GameSetup. The startup sequence is documented on GameSetup.
/// Unlike the other platforms, run() returns once the game loop has
/// started; the game keeps running asynchronously on the main thread.
///
/// Keep the Bootstrap instance alive while the game is running, typically as
/// a member of the application delegate; it owns the game and the host until
/// the completion handler runs.
class POMDOG_EXPORT Bootstrap final {
public:
    /// Sets the native window the game renders into. Required.
    void setWindow(NSWindow* window);

    /// Sets the handler invoked after the game loop has finished. When not
    /// set, the window is closed and the application is terminated.
    void onCompleted(std::function<void()>&& onCompleted);

    /// Starts the game application. On success returns nullptr and the game
    /// keeps running asynchronously; on a startup failure returns the error.
    [[nodiscard]] std::unique_ptr<Error>
    run(std::unique_ptr<GameSetup>&& gameSetup);

private:
    std::function<void()> onCompleted_;
    std::shared_ptr<pomdog::detail::cocoa::GameHostCocoa> gameHostCocoa_;
    std::shared_ptr<pomdog::detail::cocoa::GameHostMetal> gameHostMetal_;
    std::shared_ptr<Game> game_;
    std::unique_ptr<GameSetup> gameSetup_;
    __weak NSWindow* nativeWindow_ = nil;
    PomdogMetalViewController* viewController_ = nil;
};

} // namespace pomdog::cocoa
