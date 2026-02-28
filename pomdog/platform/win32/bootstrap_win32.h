// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/graphics_backend.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/platform/win32/prerequisites_win32.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class Game;
class GameHost;
} // namespace pomdog

namespace pomdog::win32 {

class POMDOG_EXPORT Bootstrap final {
public:
    /// Sets the instance handle.
    void setInstance(HINSTANCE hInstance) noexcept;

    /// Sets the WinMain function's `nCmdShow` parameter.
    void setCommandShow(i32 cmdShow) noexcept;

    /// Sets a window's icon.
    void setIcon(HICON icon) noexcept;

    /// Sets a window's small icon.
    void setIconSmall(HICON iconSmall) noexcept;

    /// Sets the format of the back buffer.
    void setSurfaceFormat(gpu::PixelFormat surfaceFormat) noexcept;

    /// Sets the format of the depth stencil buffer.
    void setDepthFormat(gpu::PixelFormat depthFormat) noexcept;

    /// Sets the maximum rate at which the back buffers can be presented. 60 by default.
    void setPresentationInterval(i32 presentationInterval) noexcept;

    /// Sets the size of the swap chain, in pixels.
    void setBackBufferSize(i32 width, i32 height) noexcept;

    /// Sets to true if the window is in full screen mode, false if it is window mode. false by default.
    void setFullScreen(bool isFullScreen) noexcept;

    /// Sets the graphics backend to use.
    void setGraphicsBackend(gpu::GraphicsBackend backend) noexcept;

    /// Sets an error event handler to a log stream.
    void onError(std::function<void(std::unique_ptr<Error>&& err)> onError);

    /// Validates the configuration of the bootstrap.
    [[nodiscard]] std::unique_ptr<Error>
    validate() noexcept;

    /// Begins running a game loop.
    void run(const std::function<std::unique_ptr<Game>(const std::shared_ptr<GameHost>&)>& createGame);

private:
    std::function<void(std::unique_ptr<Error>&& err)> onError_;
    HINSTANCE hInstance_ = nullptr;
    i32 cmdShow_ = SW_SHOWDEFAULT;
    HICON icon_ = nullptr;
    HICON iconSmall_ = nullptr;
    i32 presentationInterval_ = 60;
    i32 backBufferWidth_ = 800;
    i32 backBufferHeight_ = 480;
    gpu::PixelFormat surfaceFormat_ = gpu::PixelFormat::R8G8B8A8_UNorm;
    gpu::PixelFormat depthFormat_ = gpu::PixelFormat::Depth24Stencil8;
    gpu::GraphicsBackend graphicsBackend_ = gpu::GraphicsBackend::Direct3D11;
    bool isFullScreen_ = false;
};

} // namespace pomdog::win32
