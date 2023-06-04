// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/pixel_format.h"

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

    /// Sets to true if OpenGL is enabled, false if Metal is enabled. false by default.
    void setOpenGLEnabled(bool enabled);

    /// Sets the format of the back buffer when using the OpenGL renderer.
    void setOpenGLSurfaceFormat(PixelFormat surfaceFormat);

    /// Sets the format of the depth stencil buffer when using the OpenGL renderer.
    void setOpenGLDepthFormat(PixelFormat depthFormat);

    /// Sets an error event handler to a log stream.
    void onError(std::function<void(std::unique_ptr<Error>&& err)>&& onError);

    /// Sets an completion event handler to a log stream.
    void onCompleted(std::function<void()>&& onCompleted);

    /// Begins running a game loop.
    [[nodiscard]] std::unique_ptr<Error>
    run(std::function<std::shared_ptr<Game>(const std::shared_ptr<GameHost>&)>&& createGame);

private:
    std::function<void()> onCompleted_;
    std::function<void(std::unique_ptr<Error>&& err)> onError_;
    std::shared_ptr<pomdog::detail::cocoa::GameHostCocoa> gameHostCocoa_;
    std::shared_ptr<pomdog::detail::cocoa::GameHostMetal> gameHostMetal_;
    std::shared_ptr<Game> game_;
    __weak NSWindow* nativeWindow_ = nil;
    PomdogMetalViewController* viewController_ = nil;
    PixelFormat surfaceFormat_ = PixelFormat::R8G8B8A8_UNorm;
    PixelFormat depthFormat_ = PixelFormat::Depth24Stencil8;
    bool openGLEnabled_ = false;
};

} // namespace pomdog::cocoa
