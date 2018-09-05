// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem.GL4/OpenGLContext.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include <functional>
#include <memory>
#include <type_traits>

namespace Pomdog {
namespace Detail {
namespace Win32 {

class OpenGLContextWin32 final : public GL4::OpenGLContext {
public:
    OpenGLContextWin32() = delete;
    OpenGLContextWin32(const OpenGLContextWin32&) = delete;
    OpenGLContextWin32& operator=(const OpenGLContextWin32&) = delete;

    OpenGLContextWin32(
        HWND windowHandle,
        const PresentationParameters& presentationParameters);

    ~OpenGLContextWin32();

    void MakeCurrent() override;

    void ClearCurrent() override;

    void SwapBuffers() override;

private:
    HWND windowHandle;

    std::unique_ptr<
        std::remove_pointer<HDC>::type,
        std::function<void(HDC)>
    > hdc;

    std::unique_ptr<
        std::remove_pointer<HGLRC>::type,
        std::function<void(HGLRC)>
    > glrc;
};

} // namespace Win32
} // namespace Detail
} // namespace Pomdog
