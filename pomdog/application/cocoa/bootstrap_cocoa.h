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

class POMDOG_EXPORT Bootstrap final {
public:
    /// Sets the window where the video is drawn.
    void setWindow(NSWindow* window);

    /// Sets an error event handler to a log stream.
    void onError(std::function<void(std::unique_ptr<Error>&& err)>&& onError);

    /// Sets an completion event handler to a log stream.
    void onCompleted(std::function<void()>&& onCompleted);

    /// Begins running a game loop using a GameSetup.
    [[nodiscard]] std::unique_ptr<Error>
    run(std::unique_ptr<GameSetup>&& gameSetup);

private:
    std::function<void()> onCompleted_;
    std::function<void(std::unique_ptr<Error>&& err)> onError_;
    std::shared_ptr<pomdog::detail::cocoa::GameHostCocoa> gameHostCocoa_;
    std::shared_ptr<pomdog::detail::cocoa::GameHostMetal> gameHostMetal_;
    std::shared_ptr<Game> game_;
    std::unique_ptr<GameSetup> gameSetup_;
    __weak NSWindow* nativeWindow_ = nil;
    PomdogMetalViewController* viewController_ = nil;
};

} // namespace pomdog::cocoa
