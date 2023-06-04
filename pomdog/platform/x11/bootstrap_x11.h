// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/pixel_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class Game;
class GameHost;
} // namespace pomdog

namespace pomdog::x11 {

class POMDOG_EXPORT Bootstrap final {
public:
    /// Sets the format of the back buffer.
    void setSurfaceFormat(PixelFormat surfaceFormat) noexcept;

    /// Sets the format of the depth stencil buffer.
    void setDepthFormat(PixelFormat depthFormat) noexcept;

    /// Sets the maximum rate at which the back buffers can be presented. 60 by default.
    void setPresentationInterval(int presentationInterval) noexcept;

    /// Sets the size of the swap chain, in pixels.
    void setBackBufferSize(int width, int height) noexcept;

    /// Sets to true if the window is in full screen mode, false if it is window mode. false by default.
    void setFullScreen(bool isFullScreen) noexcept;

    /// Sets to true if OpenGL is enabled, false if Direct3D is enabled. false by default.
    void setOpenGLEnabled(bool enabled) noexcept;

    /// Sets an error event handler to a log stream.
    void onError(std::function<void(std::unique_ptr<Error>&& err)> onError);

    /// Begins running a game loop.
    void run(const std::function<std::unique_ptr<Game>(const std::shared_ptr<GameHost>&)>& createGame);

private:
    std::function<void(std::unique_ptr<Error>&& err)> onError_;
    int presentationInterval_ = 60;
    int backBufferWidth_ = 800;
    int backBufferHeight_ = 480;
    PixelFormat surfaceFormat_ = PixelFormat::R8G8B8A8_UNorm;
    PixelFormat depthFormat_ = PixelFormat::Depth24Stencil8;
    bool isFullScreen_ = false;
};

} // namespace pomdog::x11
