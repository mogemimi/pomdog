// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/gl4/opengl_context.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <GL/glx.h>
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail::x11 {
class GameWindowX11;
} // namespace pomdog::detail::x11

namespace pomdog::detail::x11 {

class OpenGLContextX11 : public gpu::detail::gl4::OpenGLContext {
public:
    OpenGLContextX11() noexcept;

    ~OpenGLContextX11() noexcept override;

    [[nodiscard]] virtual bool
    isOpenGL3Supported() const noexcept = 0;

    [[nodiscard]] static std::tuple<std::shared_ptr<OpenGLContextX11>, std::unique_ptr<Error>>
    create(
        const std::shared_ptr<GameWindowX11>& window,
        const GLXFBConfig& framebufferConfig) noexcept;
};

} // namespace pomdog::detail::x11
