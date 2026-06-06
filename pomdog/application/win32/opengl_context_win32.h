// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/basic/win32/windows_headers.h"
#include "pomdog/gpu/gl4/opengl_context.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::gpu {
struct PresentationParameters;
} // namespace pomdog::gpu

namespace pomdog::detail::win32 {

class OpenGLContextWin32 : public gpu::detail::gl4::OpenGLContext {
public:
    OpenGLContextWin32() noexcept;

    virtual ~OpenGLContextWin32() noexcept;

    [[nodiscard]] static std::tuple<std::shared_ptr<OpenGLContextWin32>, std::unique_ptr<Error>>
    create(
        HWND windowHandle,
        const gpu::PresentationParameters& presentationParameters) noexcept;

    /// Queries WGL extension support. Must be called once after GLEW
    /// initialization (glewInit) so that GLEW-resolved function pointers such
    /// as `wglGetExtensionsStringEXT` are available. Detects
    /// `WGL_EXT_swap_control_tear` for adaptive V-Sync support.
    virtual void
    initializeExtensions() noexcept = 0;

    /// Returns the current swap interval. 0 means no V-Sync (immediate
    /// presentation), 1 or greater enables V-Sync (synchronise to the vertical
    /// refresh), and a negative value indicates adaptive V-Sync via
    /// `WGL_EXT_swap_control_tear` (V-Sync when the frame rate keeps up, tearing
    /// when a frame is late).
    [[nodiscard]] virtual i32
    getSwapInterval() const noexcept = 0;

    /// Sets the swap interval. Pass 0 for no V-Sync, 1 for V-Sync, or -1 for
    /// adaptive V-Sync (`WGL_EXT_swap_control_tear`). When `WGL_EXT_swap_control_tear`
    /// is unavailable, a negative interval is silently promoted to 1 (V-Sync) so
    /// that `getSwapInterval()` reports the effective mode.
    virtual void
    setSwapInterval(i32 interval) noexcept = 0;
};

} // namespace pomdog::detail::win32
