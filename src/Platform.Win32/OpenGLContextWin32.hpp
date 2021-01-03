// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "../Graphics.GL4/OpenGLContext.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include "Pomdog/Utility/Errors.hpp"

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
