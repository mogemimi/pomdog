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

    /// Returns the current swap interval (0 = no V-Sync, 1+ = V-Sync).
    [[nodiscard]] virtual i32
    getSwapInterval() const noexcept = 0;

    /// Sets the swap interval (0 = no V-Sync, 1 = V-Sync).
    virtual void
    setSwapInterval(i32 interval) noexcept = 0;
};

} // namespace pomdog::detail::win32
