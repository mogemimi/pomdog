// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/gl4/opengl_context.h"
#include "pomdog/platform/win32/prerequisites_win32.h"

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
};

} // namespace pomdog::detail::win32
