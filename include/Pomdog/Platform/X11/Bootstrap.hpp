// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include <functional>
#include <memory>

namespace Pomdog {

class Game;
class GameHost;

namespace X11 {

class POMDOG_EXPORT Bootstrap {
public:
    void SetSurfaceFormat(SurfaceFormat surfaceFormat) noexcept;

    void SetDepthFormat(DepthFormat depthFormat) noexcept;

    void SetPresentationInterval(int presentationInterval) noexcept;

    void SetBackBufferSize(int width, int height) noexcept;

    void SetFullScreen(bool isFullScreen) noexcept;

    void SetOpenGLEnabled(bool openGLEnabled) noexcept;

    void OnError(std::function<void(const std::exception&)> onError);

    void Run(
        const std::function<std::unique_ptr<Game>(const std::shared_ptr<GameHost>&)>& createApp);

private:
    std::function<void(const std::exception&)> onError;
    int presentationInterval = 60;
    int backBufferWidth = 800;
    int backBufferHeight = 480;
    SurfaceFormat surfaceFormat = SurfaceFormat::R8G8B8A8_UNorm;
    DepthFormat depthFormat = DepthFormat::Depth24Stencil8;
    bool isFullScreen = false;
};

} // namespace X11
} // namespace Pomdog
