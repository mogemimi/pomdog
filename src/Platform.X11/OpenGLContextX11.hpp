// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "../Graphics.GL4/OpenGLContext.hpp"
#include "../Graphics.GL4/OpenGLPrerequisites.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <GL/glx.h>
#include <memory>

namespace Pomdog::Detail::X11 {

class GameWindowX11;

class OpenGLContextX11 final : public GL4::OpenGLContext {
public:
    OpenGLContextX11() noexcept;

    ~OpenGLContextX11() noexcept override;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        const std::shared_ptr<GameWindowX11>& window,
        const GLXFBConfig& framebufferConfig) noexcept;

    void MakeCurrent() override;

    void ClearCurrent() override;

    void SwapBuffers() override;

    [[nodiscard]] bool IsOpenGL3Supported() const noexcept;

private:
    std::shared_ptr<GameWindowX11> window;
    ::GLXContext glxContext = nullptr;
    bool isOpenGL3Supported = false;
};

} // namespace Pomdog::Detail::X11
