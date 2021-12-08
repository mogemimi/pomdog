// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/gl4/opengl_context.hpp"
#include "pomdog/graphics/gl4/opengl_prerequisites.hpp"
#include "pomdog/utility/errors.hpp"
#include <GL/glx.h>
#include <memory>

namespace pomdog::detail::x11 {

class GameWindowX11;

class OpenGLContextX11 final : public gl4::OpenGLContext {
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

} // namespace pomdog::detail::x11
