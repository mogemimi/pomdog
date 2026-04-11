// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/pixel_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class Game;
class GameHost;
} // namespace pomdog

namespace pomdog::emscripten {

class POMDOG_EXPORT Bootstrap final {
public:
    /// Sets the format of the back buffer.
    void setSurfaceFormat(gpu::PixelFormat surfaceFormat) noexcept;

    /// Sets the format of the depth stencil buffer.
    void setDepthFormat(gpu::PixelFormat depthFormat) noexcept;

    /// Sets the maximum rate at which the back buffers can be presented. 60 by default.
    void setPresentationInterval(int presentationInterval) noexcept;

    /// Sets the size of the swap chain, in pixels.
    void setBackBufferSize(int width, int height) noexcept;

    /// Sets to true if the window is in full screen mode, false if it is window mode. false by default.
    void setFullScreen(bool isFullScreen) noexcept;

    /// Sets command-line arguments to pass to Game::initialize().
    void setCommandLineArgs(int argc, const char* const* argv) noexcept;

    /// Sets the target canvas element id (e.g., "#canvas").
    void setTargetCanvas(const std::string& targetCanvas) noexcept;

    /// Sets an error event handler to a log stream.
    void onError(std::function<void(std::unique_ptr<Error>&& err)> onError);

    /// Begins running a game loop.
    void run(const std::function<std::unique_ptr<Game>()>& createGame);

private:
    std::function<void(std::unique_ptr<Error>&& err)> onError_;
    std::string targetCanvas_ = "#canvas";
    int presentationInterval_ = 60;
    int backBufferWidth_ = 800;
    int backBufferHeight_ = 480;
    gpu::PixelFormat surfaceFormat_ = gpu::PixelFormat::R8G8B8A8_UNorm;
    gpu::PixelFormat depthFormat_ = gpu::PixelFormat::Depth24Stencil8;
    bool isFullScreen_ = false;
    int argc_ = 0;
    const char* const* argv_ = nullptr;
};

} // namespace pomdog::emscripten
