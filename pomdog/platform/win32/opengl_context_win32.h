// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/gl4/opengl_context.h"
#include "pomdog/platform/win32/prerequisites_win32.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::win32 {

class OpenGLContextWin32 final : public gpu::detail::gl4::OpenGLContext {
public:
    OpenGLContextWin32() noexcept;

    ~OpenGLContextWin32() noexcept;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        HWND windowHandle,
        const gpu::PresentationParameters& presentationParameters) noexcept;

    void MakeCurrent() override;

    void ClearCurrent() override;

    void SwapBuffers() override;

private:
    HWND windowHandle = nullptr;

    std::unique_ptr<
        std::remove_pointer<HDC>::type,
        std::function<void(HDC)>>
        hdc;

    std::unique_ptr<
        std::remove_pointer<HGLRC>::type,
        std::function<void(HGLRC)>>
        glrc;
};

} // namespace pomdog::detail::win32
