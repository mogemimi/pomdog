// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.hpp"
#include "pomdog/graphics/surface_format.hpp"
#include <functional>
#include <memory>

#import <AppKit/NSWindow.h>

@class PomdogOpenGLView;
@class PomdogMetalViewController;

namespace Pomdog {
class Error;
class Game;
class GameHost;
} // namespace Pomdog

namespace Pomdog::Detail::Cocoa {
class GameHostCocoa;
class GameHostMetal;
} // namespace Pomdog::Detail::Cocoa

namespace Pomdog::Cocoa {

class POMDOG_EXPORT Bootstrap final {
public:
    /// Sets the window where the video is drawn.
    void SetWindow(NSWindow* window);

    /// Sets to true if OpenGL is enabled, false if Metal is enabled. false by default.
    void SetOpenGLEnabled(bool enabled);

    /// Sets the format of the back buffer when using the OpenGL renderer.
    void SetOpenGLSurfaceFormat(SurfaceFormat surfaceFormat);

    /// Sets the format of the depth stencil buffer when using the OpenGL renderer.
    void SetOpenGLDepthFormat(SurfaceFormat depthFormat);

    /// Sets an error event handler to a log stream.
    void OnError(std::function<void(std::unique_ptr<Error>&& err)>&& onError);

    /// Sets an completion event handler to a log stream.
    void OnCompleted(std::function<void()>&& onCompleted);

    /// Begins running a game loop.
    [[nodiscard]] std::unique_ptr<Error>
    Run(std::function<std::shared_ptr<Game>(const std::shared_ptr<GameHost>&)>&& createGame);

private:
    std::function<void()> onCompleted;
    std::function<void(std::unique_ptr<Error>&& err)> onError;
    std::shared_ptr<Pomdog::Detail::Cocoa::GameHostCocoa> gameHostCocoa;
    std::shared_ptr<Pomdog::Detail::Cocoa::GameHostMetal> gameHostMetal;
    std::shared_ptr<Game> game;
    __weak NSWindow* nativeWindow = nil;
    PomdogMetalViewController* viewController = nil;
    SurfaceFormat surfaceFormat = SurfaceFormat::R8G8B8A8_UNorm;
    SurfaceFormat depthFormat = SurfaceFormat::Depth24Stencil8;
    bool openGLEnabled = false;
};

} // namespace Pomdog::Cocoa
