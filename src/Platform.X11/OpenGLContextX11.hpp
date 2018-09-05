// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem.GL4/OpenGLContext.hpp"
#include "../RenderSystem.GL4/OpenGLPrerequisites.hpp"
#include <GL/glx.h>
#include <memory>

namespace Pomdog {
namespace Detail {
namespace X11 {

class GameWindowX11;

class OpenGLContextX11 final : public GL4::OpenGLContext {
public:
    OpenGLContextX11() = delete;
    OpenGLContextX11(const OpenGLContextX11&) = delete;
    OpenGLContextX11& operator=(const OpenGLContextX11&) = delete;

    OpenGLContextX11(
        const std::shared_ptr<GameWindowX11>& window,
        const GLXFBConfig& framebufferConfig);

    ~OpenGLContextX11();

    void MakeCurrent() override;

    void ClearCurrent() override;

    void SwapBuffers() override;

    bool IsOpenGL3Supported() const noexcept;

private:
    std::shared_ptr<GameWindowX11> window;
    ::GLXContext glxContext;
    bool isOpenGL3Supported;
};

} // namespace X11
} // namespace Detail
} // namespace Pomdog
