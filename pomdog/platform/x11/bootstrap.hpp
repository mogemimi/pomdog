// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.hpp"
#include "pomdog/graphics/surface_format.hpp"
#include <functional>
#include <memory>

namespace Pomdog {
class Error;
class Game;
class GameHost;
} // namespace Pomdog

namespace Pomdog::X11 {

class POMDOG_EXPORT Bootstrap final {
public:
    /// Sets the format of the back buffer.
    void SetSurfaceFormat(SurfaceFormat surfaceFormat) noexcept;

    /// Sets the format of the depth stencil buffer.
    void SetDepthFormat(SurfaceFormat depthFormat) noexcept;

    /// Sets the maximum rate at which the back buffers can be presented. 60 by default.
    void SetPresentationInterval(int presentationInterval) noexcept;

    /// Sets the size of the swap chain, in pixels.
    void SetBackBufferSize(int width, int height) noexcept;

    /// Sets to true if the window is in full screen mode, false if it is window mode. false by default.
    void SetFullScreen(bool isFullScreen) noexcept;

    /// Sets to true if OpenGL is enabled, false if Direct3D is enabled. false by default.
    void SetOpenGLEnabled(bool enabled) noexcept;

    /// Sets an error event handler to a log stream.
    void OnError(std::function<void(std::unique_ptr<Error>&& err)> onError);

    /// Begins running a game loop.
    void Run(const std::function<std::unique_ptr<Game>(const std::shared_ptr<GameHost>&)>& createGame);

private:
    std::function<void(std::unique_ptr<Error>&& err)> onError;
    int presentationInterval = 60;
    int backBufferWidth = 800;
    int backBufferHeight = 480;
    SurfaceFormat surfaceFormat = SurfaceFormat::R8G8B8A8_UNorm;
    SurfaceFormat depthFormat = SurfaceFormat::Depth24Stencil8;
    bool isFullScreen = false;
};

} // namespace Pomdog::X11
