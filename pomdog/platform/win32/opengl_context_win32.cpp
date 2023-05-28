// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/platform/win32/opengl_context_win32.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::win32 {
namespace {

[[nodiscard]] std::unique_ptr<Error>
toPixelFormatDescriptor(
    const gpu::PresentationParameters& presentationParameters,
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
        0, 0, 0};

    switch (presentationParameters.backBufferFormat) {
    case PixelFormat::R8G8B8A8_UNorm:
        descriptor.cColorBits = 32;
        break;
    case PixelFormat::B8G8R8A8_UNorm:
        descriptor.cColorBits = 32;
        break;
    case PixelFormat::R16G16B16A16_Float:
        descriptor.cColorBits = 64;
        break;
    case PixelFormat::R32G32B32A32_Float:
        descriptor.cColorBits = 128;
        break;
    default:
        return errors::make("invalid back buffer format");
    }

    switch (presentationParameters.depthStencilFormat) {
    case PixelFormat::Depth16:
        descriptor.cDepthBits = 16;
        descriptor.cStencilBits = 0;
        break;
    case PixelFormat::Depth24Stencil8:
        descriptor.cDepthBits = 24;
        descriptor.cStencilBits = 8;
        break;
    case PixelFormat::Depth32:
        descriptor.cDepthBits = 32;
        descriptor.cStencilBits = 0;
        break;
    case PixelFormat::Depth32_Float_Stencil8_Uint:
        descriptor.cDepthBits = 32;
        descriptor.cStencilBits = 8;
        break;
    case PixelFormat::Invalid:
        descriptor.cDepthBits = 0;
        descriptor.cStencilBits = 0;
        break;
    default:
        return errors::make("invalid depth stencil format");
    }

    return nullptr;
}

} // namespace

OpenGLContextWin32::OpenGLContextWin32() noexcept = default;

std::unique_ptr<Error>
OpenGLContextWin32::initialize(
    HWND windowHandleIn,
    const gpu::PresentationParameters& presentationParameters) noexcept
{
    windowHandle_ = windowHandleIn;
    if (windowHandle_ == nullptr) {
        return errors::make("windowHandle must be != nullptr");
    }

    using hdcType = decltype(hdc_);
    hdc_ = hdcType(nullptr, [this](HDC hdcIn) {
        ::ReleaseDC(windowHandle_, hdcIn);
    });

    using glrcType = decltype(glrc_);
    glrc_ = glrcType(nullptr, [](HGLRC glrcIn) {
        if (::wglGetCurrentContext() == glrcIn) {
            ::wglMakeCurrent(nullptr, nullptr);
        }
        ::wglDeleteContext(glrcIn);
    });

    hdc_.reset(::GetDC(windowHandle_));

    PIXELFORMATDESCRIPTOR formatDescriptor;
    if (auto err = toPixelFormatDescriptor(presentationParameters, formatDescriptor); err != nullptr) {
        return errors::wrap(std::move(err), "toPixelFormatDescriptor() failed");
    }

    const auto pixelFormat = ::ChoosePixelFormat(hdc_.get(), &formatDescriptor);

    if (pixelFormat == 0) {
        const auto errorCode = ::GetLastError();
        return errors::make("ChoosePixelFormat() failed. error code = " + std::to_string(errorCode));
    }

    if (!SetPixelFormat(hdc_.get(), pixelFormat, &formatDescriptor)) {
        const auto errorCode = ::GetLastError();
        return errors::make("SetPixelFormat() failed. error code = " + std::to_string(errorCode));
    }

    // NOTE: Create OpenGL context.
    glrc_.reset(::wglCreateContext(hdc_.get()));

    if (!::wglMakeCurrent(hdc_.get(), glrc_.get())) {
        const auto errorCode = ::GetLastError();
        return errors::make("wglMakeCurrent() failed. error code = " + std::to_string(errorCode));
    }

    return nullptr;
}

OpenGLContextWin32::~OpenGLContextWin32() noexcept
{
    glrc_.reset();
    hdc_.reset();
}

void OpenGLContextWin32::makeCurrent()
{
    POMDOG_ASSERT(hdc_);
    POMDOG_ASSERT(glrc_);
    ::wglMakeCurrent(hdc_.get(), glrc_.get());
}

void OpenGLContextWin32::clearCurrent()
{
    ::wglMakeCurrent(nullptr, nullptr);
}

void OpenGLContextWin32::swapBuffers()
{
    ::glFlush();
    POMDOG_CHECK_ERROR_GL4("glFlush");

    POMDOG_ASSERT(hdc_);
    ::SwapBuffers(hdc_.get());
}

} // namespace pomdog::detail::win32
