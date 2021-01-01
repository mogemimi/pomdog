// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {
class Error;
class Game;
class GameHost;
} // namespace Pomdog

namespace Pomdog::Win32 {

class POMDOG_EXPORT Bootstrap final {
public:
    /// Sets the instance handle.
    void SetInstance(HINSTANCE hInstance) noexcept;

    /// Sets the WinMain function's `nCmdShow` parameter.
    void SetCommandShow(int cmdShow) noexcept;

    /// Sets a window's icon.
    void SetIcon(HICON icon) noexcept;

    /// Sets a window's small icon.
    void SetIconSmall(HICON iconSmall) noexcept;

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
    HINSTANCE hInstance = nullptr;
    int cmdShow = SW_SHOWDEFAULT;
    HICON icon = nullptr;
    HICON iconSmall = nullptr;
    int presentationInterval = 60;
    int backBufferWidth = 800;
    int backBufferHeight = 480;
    SurfaceFormat surfaceFormat = SurfaceFormat::R8G8B8A8_UNorm;
    SurfaceFormat depthFormat = SurfaceFormat::Depth24Stencil8;
    bool isFullScreen = false;
    bool openGLEnabled = false;
};

} // namespace Pomdog::Win32
