// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "OpenGLContextWin32.hpp"
#include "../RenderSystem.GL4/ErrorChecker.hpp"
#include "../RenderSystem.GL4/OpenGLPrerequisites.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <sstream>
#include <utility>

namespace Pomdog {
namespace Detail {
namespace Win32 {
namespace {

PIXELFORMATDESCRIPTOR ToPixelFormatDescriptor(
    const PresentationParameters& presentationParameters)
{
    PIXELFORMATDESCRIPTOR descriptor =
    {
        sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,
        0, 0, 0, 0, 0, 0, 0, 0,
        0,
        0, 0, 0, 0,
        24,
        8,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    switch (presentationParameters.BackBufferFormat) {
    case SurfaceFormat::R8G8B8A8_UNorm:
        descriptor.cColorBits = 32;
        break;
    case SurfaceFormat::R16G16B16A16_Float:
        descriptor.cColorBits = 64;
        break;
    case SurfaceFormat::R32G32B32A32_Float:
        descriptor.cColorBits = 128;
        break;
    default:
        descriptor.cColorBits = 32;
        break;
    }

    switch (presentationParameters.DepthStencilFormat) {
    case DepthFormat::Depth16:
        descriptor.cDepthBits = 16;
        descriptor.cStencilBits = 0;
        break;
    case DepthFormat::Depth24Stencil8:
        descriptor.cDepthBits = 24;
        descriptor.cStencilBits = 8;
        break;
    case DepthFormat::Depth32:
        descriptor.cDepthBits = 32;
        descriptor.cStencilBits = 0;
        break;
    case DepthFormat::Depth32_Float_Stencil8_Uint:
        descriptor.cDepthBits = 32;
        descriptor.cStencilBits = 8;
        break;
    case DepthFormat::None:
        descriptor.cDepthBits = 0;
        descriptor.cStencilBits = 0;
        break;
    }

    return std::move(descriptor);
}

} // unnamed namespace

OpenGLContextWin32::OpenGLContextWin32(
    HWND windowHandleIn,
    const PresentationParameters& presentationParameters)
    : windowHandle(windowHandleIn)
    , hdc(nullptr, [this](HDC hdcIn) {
        ReleaseDC(windowHandle, hdcIn);
    })
    , glrc(nullptr, [](HGLRC glrcIn) {
        if (wglGetCurrentContext() == glrcIn) {
            wglMakeCurrent(nullptr, nullptr);
        }
        wglDeleteContext(glrcIn);
    })
{
    hdc.reset(GetDC(windowHandle));

    auto formatDescriptor = ToPixelFormatDescriptor(presentationParameters);

    auto const pixelFormat = ChoosePixelFormat(hdc.get(), &formatDescriptor);

    if (0 == pixelFormat) {
        auto const errorCode = ::GetLastError();

        std::stringstream ss;
        ss << "Failed to call ChoosePixelFormat function."
            << "The calling thread's last-error code is "
            << std::hex << errorCode;

        POMDOG_THROW_EXCEPTION(std::runtime_error, ss.str());
    }

    if (!SetPixelFormat(hdc.get(), pixelFormat, &formatDescriptor)) {
        auto const errorCode = ::GetLastError();

        std::stringstream ss;
        ss << "Failed to call SetPixelFormat function."
            << "The calling thread's last-error code is "
            << std::hex << errorCode;

        POMDOG_THROW_EXCEPTION(std::runtime_error, ss.str());
    }

    // Create gl context
    glrc.reset(wglCreateContext(hdc.get()));

    if (!wglMakeCurrent(hdc.get(), glrc.get())) {
        auto const errorCode = ::GetLastError();

        std::stringstream ss;
        ss << "Failed to call wglMakeCurrent function."
            << "The calling thread's last-error code is "
            << std::hex << errorCode;

        POMDOG_THROW_EXCEPTION(std::runtime_error, ss.str());
    }
}

OpenGLContextWin32::~OpenGLContextWin32()
{
    glrc.reset();
    hdc.reset();
}

void OpenGLContextWin32::MakeCurrent()
{
    POMDOG_ASSERT(hdc);
    POMDOG_ASSERT(glrc);
    wglMakeCurrent(hdc.get(), glrc.get());
}

void OpenGLContextWin32::ClearCurrent()
{
    wglMakeCurrent(nullptr, nullptr);
}

void OpenGLContextWin32::SwapBuffers()
{
    glFlush();
    POMDOG_CHECK_ERROR_GL4("glFlush");

    POMDOG_ASSERT(hdc);
    ::SwapBuffers(hdc.get());
}

} // namespace Win32
} // namespace Detail
} // namespace Pomdog
