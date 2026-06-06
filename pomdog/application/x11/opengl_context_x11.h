// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
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

    /// Returns the current swap interval. 0 means no V-Sync (immediate
    /// presentation), 1 or greater enables V-Sync (synchronise to the vertical
    /// refresh), and a negative value indicates adaptive V-Sync via
    /// `GLX_EXT_swap_control_tear` (V-Sync when the frame rate keeps up, tearing
    /// when a frame is late).
    [[nodiscard]] virtual i32
    getSwapInterval() const noexcept = 0;

    /// Sets the swap interval. Pass 0 for no V-Sync, 1 for V-Sync, or -1 for
    /// adaptive V-Sync (`GLX_EXT_swap_control_tear`). When `GLX_EXT_swap_control_tear`
    /// is unavailable, a negative interval is silently promoted to 1 (V-Sync) so
    /// that `getSwapInterval()` reports the effective mode.
    virtual void
    setSwapInterval(i32 interval) noexcept = 0;

    [[nodiscard]] static std::tuple<std::shared_ptr<OpenGLContextX11>, std::unique_ptr<Error>>
    create(
        const std::shared_ptr<GameWindowX11>& window,
        const GLXFBConfig& framebufferConfig) noexcept;
};

} // namespace pomdog::detail::x11
