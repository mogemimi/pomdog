// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/gl4/opengl_context.hpp"
#include "pomdog/platform/win32/prerequisites_win32.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Win32 {

class OpenGLContextWin32 final : public GL4::OpenGLContext {
public:
    OpenGLContextWin32() noexcept;

    ~OpenGLContextWin32() noexcept;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        HWND windowHandle,
        const PresentationParameters& presentationParameters) noexcept;

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

} // namespace Pomdog::Detail::Win32
