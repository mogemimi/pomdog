// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "OpenGLContextX11.hpp"
#include "GameWindowX11.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <X11/Xlib.h>
#include <cstring>
#include <utility>

namespace Pomdog::Detail::X11 {
namespace {

int trappedErrorCode = 0;

[[nodiscard]] int
GLContextErrorHandler([[maybe_unused]] Display* display, XErrorEvent* error) noexcept
{
    trappedErrorCode = error->error_code;
    return 0;
}

[[nodiscard]] XErrorHandler
TrapErrors(XErrorHandler errorHandler) noexcept
{
    trappedErrorCode = 0;
    return XSetErrorHandler(errorHandler);
}

[[nodiscard]] int
UntrapErrors(XErrorHandler oldErrorHandler) noexcept
{
    XSetErrorHandler(oldErrorHandler);
    return trappedErrorCode;
}

[[nodiscard]] bool
IsExtensionSupported(const char* extensionList, const char* extension) noexcept
{
    POMDOG_ASSERT(extensionList != nullptr);
    POMDOG_ASSERT(extension != nullptr);
    POMDOG_ASSERT(*extension != '\0');
    POMDOG_ASSERT(std::strchr(extension, ' ') == nullptr);

    for (const char* start = extensionList;;) {
        auto where = std::strstr(start, extension);

        if (where == nullptr) {
            break;
        }

        auto terminator = where + std::strlen(extension);

        if (where == start || *(where - 1) == ' ') {
            if (*terminator == ' ' || *terminator == '\0') {
                return true;
            }
        }

        start = terminator;
    }

    return false;
}

} // namespace

OpenGLContextX11::OpenGLContextX11() noexcept = default;

std::unique_ptr<Error>
OpenGLContextX11::Initialize(
    const std::shared_ptr<GameWindowX11>& windowIn,
    const GLXFBConfig& framebufferConfig) noexcept
{
    window = windowIn;
    glxContext = nullptr;
    isOpenGL3Supported = false;

    POMDOG_ASSERT(framebufferConfig != nullptr);
    POMDOG_ASSERT(window != nullptr);

    auto display = window->GetNativeDisplay();

    auto glxExtensionsString = glXQueryExtensionsString(display, DefaultScreen(display));

    auto glXCreateContextAttribsARB = reinterpret_cast<PFNGLXCREATECONTEXTATTRIBSARBPROC>(
        glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB")));

    if (!IsExtensionSupported(glxExtensionsString, "GLX_ARB_create_context") ||
        glXCreateContextAttribsARB == nullptr) {
        // NOTE:
        // glXCreateContextAttribsARB() not found
        // ... This platform is not supported OpenGL 3.0 or later.
        // ... using old-style GLX context
        auto oldHandler = TrapErrors(GLContextErrorHandler);
        glxContext = glXCreateNewContext(
            display,
            framebufferConfig,
            GLX_RGBA_TYPE,
            nullptr,
            True);
        XSync(display, False);
        if (auto errorCode = UntrapErrors(oldHandler); errorCode != 0) {
            return Errors::New("glXCreateNewContext() failed: error code = " + std::to_string(errorCode));
        }
    }
    else {
        const int configs[][8] = {
            {
                // NOTE: OpenGL 4.5
                GLX_CONTEXT_MAJOR_VERSION_ARB,
                4,
                GLX_CONTEXT_MINOR_VERSION_ARB,
                5,
                // GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
                // GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                None,
            },
            {
                // NOTE: OpenGL 4.1
                GLX_CONTEXT_MAJOR_VERSION_ARB,
                4,
                GLX_CONTEXT_MINOR_VERSION_ARB,
                1,
                None,
            },
            {
                // NOTE: OpenGL 3.3
                GLX_CONTEXT_MAJOR_VERSION_ARB,
                3,
                GLX_CONTEXT_MINOR_VERSION_ARB,
                3,
                None,
            },
            {
                // NOTE: OpenGL 2.1
                GLX_CONTEXT_MAJOR_VERSION_ARB,
                2,
                GLX_CONTEXT_MINOR_VERSION_ARB,
                1,
                None,
            },
        };

        int lastError = 0;
        for (const auto& attribs : configs) {
            auto oldHandler = TrapErrors(GLContextErrorHandler);
            glxContext = glXCreateContextAttribsARB(
                display,
                framebufferConfig,
                nullptr,
                True,
                attribs);
            XSync(display, False);
            if (lastError = UntrapErrors(oldHandler); (lastError == 0) && (glxContext != nullptr)) {
                assert(attribs[0] == GLX_CONTEXT_MAJOR_VERSION_ARB);
                assert(attribs[1] >= 2);
                if (attribs[1] >= 3) {
                    isOpenGL3Supported = true;
                }
                break;
            }
        }

        if (lastError != 0) {
            return Errors::New("glXCreateContextAttribsARB() failed: error code = " + std::to_string(lastError));
        }
    }

    if (glxContext == nullptr) {
        return Errors::New("glxContext is nullptr");
    }

    return nullptr;
}

OpenGLContextX11::~OpenGLContextX11() noexcept
{
    if (glxContext != nullptr) {
        POMDOG_ASSERT(window != nullptr);
        auto display = window->GetNativeDisplay();

        if (glXGetCurrentContext() == glxContext) {
            glXMakeCurrent(display, None, nullptr);
        }
        glXDestroyContext(display, glxContext);
    }
}

void OpenGLContextX11::MakeCurrent()
{
    if (glXGetCurrentContext() == glxContext) {
        return;
    }

    POMDOG_ASSERT(window != nullptr);
    glXMakeCurrent(window->GetNativeDisplay(), window->GetNativeWindow(), glxContext);
}

void OpenGLContextX11::ClearCurrent()
{
    POMDOG_ASSERT(window != nullptr);
    glXMakeCurrent(window->GetNativeDisplay(), None, nullptr);
}

void OpenGLContextX11::SwapBuffers()
{
    POMDOG_ASSERT(window != nullptr);
    glXSwapBuffers(window->GetNativeDisplay(), window->GetNativeWindow());
}

bool OpenGLContextX11::IsOpenGL3Supported() const noexcept
{
    return isOpenGL3Supported;
}

} // namespace Pomdog::Detail::X11
