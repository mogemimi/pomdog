// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/platform/win32/opengl_context_win32.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/graphics/gl4/error_checker.hpp"
#include "pomdog/graphics/gl4/opengl_prerequisites.hpp"
#include "pomdog/graphics/presentation_parameters.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Win32 {
namespace {

[[nodiscard]] std::unique_ptr<Error>
ToPixelFormatDescriptor(
    const PresentationParameters& presentationParameters,
    PIXELFORMATDESCRIPTOR& descriptor) noexcept
{
    descriptor = PIXELFORMATDESCRIPTOR{
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
    case SurfaceFormat::B8G8R8A8_UNorm:
        descriptor.cColorBits = 32;
        break;
    case SurfaceFormat::R16G16B16A16_Float:
        descriptor.cColorBits = 64;
        break;
    case SurfaceFormat::R32G32B32A32_Float:
        descriptor.cColorBits = 128;
        break;
    default:
        return Errors::New("invalid back buffer format");
    }

    switch (presentationParameters.DepthStencilFormat) {
    case SurfaceFormat::Depth16:
        descriptor.cDepthBits = 16;
        descriptor.cStencilBits = 0;
        break;
    case SurfaceFormat::Depth24Stencil8:
        descriptor.cDepthBits = 24;
        descriptor.cStencilBits = 8;
        break;
    case SurfaceFormat::Depth32:
        descriptor.cDepthBits = 32;
        descriptor.cStencilBits = 0;
        break;
    case SurfaceFormat::Depth32_Float_Stencil8_Uint:
        descriptor.cDepthBits = 32;
        descriptor.cStencilBits = 8;
        break;
    case SurfaceFormat::Invalid:
        descriptor.cDepthBits = 0;
        descriptor.cStencilBits = 0;
        break;
    default:
        return Errors::New("invalid depth stencil format");
    }

    return nullptr;
}

} // namespace

OpenGLContextWin32::OpenGLContextWin32() noexcept = default;

std::unique_ptr<Error>
OpenGLContextWin32::Initialize(
    HWND windowHandleIn,
    const PresentationParameters& presentationParameters) noexcept
{
    windowHandle = windowHandleIn;
    if (windowHandle == nullptr) {
        return Errors::New("windowHandle must be != nullptr");
    }

    using hdcType = decltype(hdc);
    hdc = hdcType(nullptr, [this](HDC hdcIn) {
        ::ReleaseDC(windowHandle, hdcIn);
    });

    using glrcType = decltype(glrc);
    glrc = glrcType(nullptr, [](HGLRC glrcIn) {
        if (::wglGetCurrentContext() == glrcIn) {
            ::wglMakeCurrent(nullptr, nullptr);
        }
        ::wglDeleteContext(glrcIn);
    });

    hdc.reset(::GetDC(windowHandle));

    PIXELFORMATDESCRIPTOR formatDescriptor;
    if (auto err = ToPixelFormatDescriptor(presentationParameters, formatDescriptor); err != nullptr) {
        return Errors::Wrap(std::move(err), "ToPixelFormatDescriptor() failed");
    }

    const auto pixelFormat = ::ChoosePixelFormat(hdc.get(), &formatDescriptor);

    if (pixelFormat == 0) {
        const auto errorCode = ::GetLastError();
        return Errors::New("ChoosePixelFormat() failed. error code = " + std::to_string(errorCode));
    }

    if (!SetPixelFormat(hdc.get(), pixelFormat, &formatDescriptor)) {
        const auto errorCode = ::GetLastError();
        return Errors::New("SetPixelFormat() failed. error code = " + std::to_string(errorCode));
    }

    // NOTE: Create OpenGL context.
    glrc.reset(::wglCreateContext(hdc.get()));

    if (!::wglMakeCurrent(hdc.get(), glrc.get())) {
        const auto errorCode = ::GetLastError();
        return Errors::New("wglMakeCurrent() failed. error code = " + std::to_string(errorCode));
    }

    return nullptr;
}

OpenGLContextWin32::~OpenGLContextWin32() noexcept
{
    glrc.reset();
    hdc.reset();
}

void OpenGLContextWin32::MakeCurrent()
{
    POMDOG_ASSERT(hdc);
    POMDOG_ASSERT(glrc);
    ::wglMakeCurrent(hdc.get(), glrc.get());
}

void OpenGLContextWin32::ClearCurrent()
{
    ::wglMakeCurrent(nullptr, nullptr);
}

void OpenGLContextWin32::SwapBuffers()
{
    ::glFlush();
    POMDOG_CHECK_ERROR_GL4("glFlush");

    POMDOG_ASSERT(hdc);
    ::SwapBuffers(hdc.get());
}

} // namespace Pomdog::Detail::Win32
