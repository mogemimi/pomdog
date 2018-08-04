// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "OpenGLContextX11.hpp"
#include "GameWindowX11.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <X11/Xlib.h>
#include <cstring>

namespace Pomdog {
namespace Detail {
namespace X11 {
namespace {

class GLContextErrorHelper final {
private:
    static bool contextErrorOccurred;

public:
    static int ContextErrorHandler(Display*, XErrorEvent*) noexcept
    {
        contextErrorOccurred = true;
        return 0;
    }

    static void Reset() noexcept
    {
        contextErrorOccurred = false;
    }

    static bool HasError() noexcept
    {
        return contextErrorOccurred;
    }
};

bool GLContextErrorHelper::contextErrorOccurred = false;

static bool IsExtensionSupported(const char* extensionList, const char* extension)
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

} // unnamed namespace

OpenGLContextX11::OpenGLContextX11(
    const std::shared_ptr<GameWindowX11>& windowIn,
    const GLXFBConfig& framebufferConfig)
    : window(windowIn)
    , glxContext(0)
    , isOpenGL3Supported(false)
{
    POMDOG_ASSERT(framebufferConfig != nullptr);
    POMDOG_ASSERT(window);

    auto display = window->NativeDisplay();

    auto glxExtensionsString = glXQueryExtensionsString(display, DefaultScreen(display));

    auto glXCreateContextAttribsARB = reinterpret_cast<PFNGLXCREATECONTEXTATTRIBSARBPROC>(
        glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB")));

    GLContextErrorHelper::Reset();
    XErrorHandler oldHandler = XSetErrorHandler(GLContextErrorHelper::ContextErrorHandler);

    if (!IsExtensionSupported(glxExtensionsString, "GLX_ARB_create_context")
        || glXCreateContextAttribsARB == nullptr) {
        ///@note
        /// glXCreateContextAttribsARB() not found
        /// ... This platform is not supported OpenGL 3.0 or later.
        /// ... using old-style GLX context
        glxContext = glXCreateNewContext(
            display, framebufferConfig, GLX_RGBA_TYPE, 0, True);
    }
    else {
        int contextAttributes[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 3,
            //GLX_CONTEXT_FLAGS_ARB, 0,
            //GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            //GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
            None,
        };

        glxContext = glXCreateContextAttribsARB(
            display, framebufferConfig, 0, True, contextAttributes);

        XSync(display, False);

        if (!GLContextErrorHelper::HasError() && glxContext != 0) {
            // OK
            isOpenGL3Supported = true;
        }
        else {
            //GLX_CONTEXT_MAJOR_VERSION_ARB = 2
            contextAttributes[1] = 2;
            //GLX_CONTEXT_MINOR_VERSION_ARB = 0;
            contextAttributes[3] = 0;

            GLContextErrorHelper::Reset();
            glxContext = glXCreateContextAttribsARB(
                display, framebufferConfig, 0, True, contextAttributes);
        }
    }

    XSync(display, False);

    XSetErrorHandler(oldHandler);

    if (GLContextErrorHelper::HasError() || (glxContext == 0)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create an OpenGL context");
    }

    if (glxContext == nullptr) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Error: glXCreateContext");
    }
}

OpenGLContextX11::~OpenGLContextX11()
{
    if (glxContext != nullptr) {
        POMDOG_ASSERT(window);
        auto display = window->NativeDisplay();

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

    POMDOG_ASSERT(window);
    glXMakeCurrent(window->NativeDisplay(), window->NativeWindow(), glxContext);
}

void OpenGLContextX11::ClearCurrent()
{
    POMDOG_ASSERT(window);
    glXMakeCurrent(window->NativeDisplay(), None, nullptr);
}

void OpenGLContextX11::SwapBuffers()
{
    POMDOG_ASSERT(window);
    glXSwapBuffers(window->NativeDisplay(), window->NativeWindow());
}

bool OpenGLContextX11::IsOpenGL3Supported() const noexcept
{
    return isOpenGL3Supported;
}

} // namespace X11
} // namespace Detail
} // namespace Pomdog
