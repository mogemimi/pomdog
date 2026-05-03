// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/x11/opengl_context_x11.h"
#include "pomdog/application/x11/game_window_x11.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <X11/Xlib.h>
#include <atomic>
#include <cstring>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::x11 {
namespace {

std::atomic<int> trappedErrorCode = 0;

[[nodiscard]] int
GLContextErrorHandler([[maybe_unused]] Display* display, XErrorEvent* error) noexcept
{
    trappedErrorCode = error->error_code;
    return 0;
}

[[nodiscard]] XErrorHandler
trapErrors(XErrorHandler errorHandler) noexcept
{
    trappedErrorCode = 0;
    return XSetErrorHandler(errorHandler);
}

[[nodiscard]] int
untrapErrors(XErrorHandler oldErrorHandler) noexcept
{
    XSetErrorHandler(oldErrorHandler);
    return trappedErrorCode;
}

[[nodiscard]] bool
isExtensionSupported(const char* extensions, const char* name) noexcept
{
    if (extensions == nullptr) {
        return false;
    }
    if (name == nullptr || *name == '\0') {
        return false;
    }

    POMDOG_ASSERT(name != nullptr);
    POMDOG_ASSERT(*name != '\0');
    POMDOG_ASSERT(std::strchr(name, ' ') == nullptr);

    const auto nameLen = std::strlen(name);

    for (const char* start = extensions;;) {
        auto where = std::strstr(start, name);

        if (where == nullptr) {
            break;
        }

        const auto terminator = where + nameLen;

        const bool begins = (where == start) || (*(where - 1) == ' ');
        const bool ends = (*terminator == ' ') || (*terminator == '\0');

        if (begins && ends) {
            return true;
        }

        start = terminator;
    }

    return false;
}

class OpenGLContextX11Impl final : public OpenGLContextX11 {
private:
    std::shared_ptr<GameWindowX11> window_;
    ::GLXContext glxContext_ = nullptr;
    ::GLXWindow glxWindow_ = 0;
    bool isOpenGL3Supported_ = false;
    bool hasSwapControlEXT_ = false;
    bool hasSwapControlMESA_ = false;
    bool hasSwapControlSGI_ = false;

public:
    ~OpenGLContextX11Impl() noexcept override
    {
        if (glxContext_ != nullptr) {
            POMDOG_ASSERT(window_ != nullptr);
            const auto display = window_->getNativeDisplay();

            if (glXGetCurrentContext() == glxContext_) {
                // NOTE: Use glXMakeContextCurrent (not the legacy glXMakeCurrent) to
                // match the paired calls in makeCurrent() / clearCurrent().
                glXMakeContextCurrent(display, None, None, nullptr);
            }
            if (glxWindow_ != 0) {
                glXDestroyWindow(display, glxWindow_);
                glxWindow_ = 0;
            }
            glXDestroyContext(display, glxContext_);
        }
    }

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<GameWindowX11>& windowIn,
        const GLXFBConfig& framebufferConfig) noexcept
    {
        window_ = windowIn;
        glxContext_ = nullptr;
        glxWindow_ = 0;
        isOpenGL3Supported_ = false;

        POMDOG_ASSERT(framebufferConfig != nullptr);
        POMDOG_ASSERT(window_ != nullptr);

        const auto display = window_->getNativeDisplay();

        const auto glxExtensionsString = glXQueryExtensionsString(display, DefaultScreen(display));

        POMDOG_CLANG_SUPPRESS_WARNING_PUSH
        POMDOG_CLANG_SUPPRESS_WARNING("-Wcast-function-type-strict")
        const auto glXCreateContextAttribsARB = reinterpret_cast<PFNGLXCREATECONTEXTATTRIBSARBPROC>(
            glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB")));
        POMDOG_CLANG_SUPPRESS_WARNING_POP

        if (!isExtensionSupported(glxExtensionsString, "GLX_ARB_create_context") ||
            glXCreateContextAttribsARB == nullptr) {
            auto oldHandler = trapErrors(GLContextErrorHandler);
            glxContext_ = glXCreateNewContext(
                display,
                framebufferConfig,
                GLX_RGBA_TYPE,
                nullptr,
                True);
            XSync(display, False);
            if (auto errorCode = untrapErrors(oldHandler); errorCode != 0) {
                return errors::make("glXCreateNewContext() failed: error code = " + std::to_string(errorCode));
            }
        }
        else {
            const int configs[][8] = {
                {
                    GLX_CONTEXT_MAJOR_VERSION_ARB,
                    4,
                    GLX_CONTEXT_MINOR_VERSION_ARB,
                    5,
                    None,
                },
                {
                    GLX_CONTEXT_MAJOR_VERSION_ARB,
                    4,
                    GLX_CONTEXT_MINOR_VERSION_ARB,
                    1,
                    None,
                },
                {
                    GLX_CONTEXT_MAJOR_VERSION_ARB,
                    3,
                    GLX_CONTEXT_MINOR_VERSION_ARB,
                    3,
                    None,
                },
                {
                    GLX_CONTEXT_MAJOR_VERSION_ARB,
                    2,
                    GLX_CONTEXT_MINOR_VERSION_ARB,
                    1,
                    None,
                },
            };

            int lastError = 0;
            for (const auto& attribs : configs) {
                auto oldHandler = trapErrors(GLContextErrorHandler);
                glxContext_ = glXCreateContextAttribsARB(
                    display,
                    framebufferConfig,
                    nullptr,
                    True,
                    attribs);
                XSync(display, False);
                if (lastError = untrapErrors(oldHandler); (lastError == 0) && (glxContext_ != nullptr)) {
                    POMDOG_ASSERT(attribs[0] == GLX_CONTEXT_MAJOR_VERSION_ARB);
                    POMDOG_ASSERT(attribs[1] >= 2);
                    if (attribs[1] >= 3) {
                        isOpenGL3Supported_ = true;
                    }
                    break;
                }
            }

            if (lastError != 0) {
                return errors::make("glXCreateContextAttribsARB() failed: error code = " + std::to_string(lastError));
            }
        }

        if (glxContext_ == nullptr) {
            return errors::make("glxContext is nullptr");
        }

        {
            int glxMajor = 0;
            int glxMinor = 0;
            if (!glXQueryVersion(display, &glxMajor, &glxMinor)) {
                return errors::make("glXQueryVersion() failed");
            }

            if (glxMajor < 1 || (glxMajor == 1 && glxMinor < 3)) {
                // NOTE: GLX version 1.3 or higher is required for glXCreateWindow and glXQueryDrawable.
                return errors::make("GLX version 1.3 or higher is required");
            }
        }

        // NOTE: GLX drawable functions (glXQueryDrawable, glXSwapIntervalEXT,
        // glXSwapBuffers, glXMakeContextCurrent) require a GLXWindow created via
        // glXCreateWindow(), NOT a raw X11 Window from XCreateWindow().
        // Passing an X11 Window where a GLXDrawable is expected causes a
        // Segmentation fault (core dumped) at runtime.
        glxWindow_ = glXCreateWindow(display, framebufferConfig, window_->getNativeWindow(), nullptr);
        if (glxWindow_ == 0) {
            return errors::make("glXCreateWindow() failed");
        }

        hasSwapControlEXT_ = isExtensionSupported(glxExtensionsString, "GLX_EXT_swap_control");
        hasSwapControlMESA_ = isExtensionSupported(glxExtensionsString, "GLX_MESA_swap_control");
        hasSwapControlSGI_ = isExtensionSupported(glxExtensionsString, "GLX_SGI_swap_control");

        return nullptr;
    }

    void
    makeCurrent() override
    {
        if (glXGetCurrentContext() == glxContext_) {
            return;
        }

        POMDOG_ASSERT(window_ != nullptr);
        glXMakeContextCurrent(window_->getNativeDisplay(), glxWindow_, glxWindow_, glxContext_);
    }

    void
    clearCurrent() override
    {
        POMDOG_ASSERT(window_ != nullptr);
        glXMakeContextCurrent(window_->getNativeDisplay(), None, None, nullptr);
    }

    void
    swapBuffers() override
    {
        POMDOG_ASSERT(window_ != nullptr);
        // NOTE: glXSwapBuffers requires a GLXDrawable. Pass `glxWindow_` (created via
        // `glXCreateWindow`) rather than the raw X11 Window from `getNativeWindow()`.
        glXSwapBuffers(window_->getNativeDisplay(), glxWindow_);
    }

    bool
    isOpenGL3Supported() const noexcept override
    {
        return isOpenGL3Supported_;
    }

    [[nodiscard]] i32
    getSwapInterval() const noexcept override
    {
        POMDOG_ASSERT(window_ != nullptr);
        const auto display = window_->getNativeDisplay();

        // NOTE: Try glXQueryDrawable (GLX_EXT_swap_control) first.
        if (hasSwapControlEXT_) {
            unsigned int swapInterval = 0;

            // NOTE: Calling glXQueryDrawable with GLX_SWAP_INTERVAL_EXT when the
            // GLX_EXT_swap_control extension is not actually supported causes a
            // Segmentation fault (core dumped).  glXGetProcAddressARB() returning a
            // non-null function pointer does NOT reliably indicate that the extension
            // is present; glXQueryExtensionsString() must be used for that check.
            // hasSwapControlEXT_ is set from glXQueryExtensionsString() in initialize().
            glXQueryDrawable(display, glxWindow_, GLX_SWAP_INTERVAL_EXT, &swapInterval);
            return static_cast<i32>(swapInterval);
        }

        // NOTE: GLX_MESA_swap_control fallback.
        if (hasSwapControlMESA_) {
            POMDOG_CLANG_SUPPRESS_WARNING_PUSH
            POMDOG_CLANG_SUPPRESS_WARNING("-Wcast-function-type-strict")
            const auto glXGetSwapIntervalMESA = reinterpret_cast<PFNGLXGETSWAPINTERVALMESAPROC>(
                glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glXGetSwapIntervalMESA")));
            POMDOG_CLANG_SUPPRESS_WARNING_POP

            if (glXGetSwapIntervalMESA != nullptr) {
                return static_cast<i32>(glXGetSwapIntervalMESA());
            }
        }

        // NOTE: Neither extension is available; V-Sync state is unknown.
        return 0;
    }

    void
    setSwapInterval(i32 interval) noexcept override
    {
        POMDOG_ASSERT(window_ != nullptr);
        const auto display = window_->getNativeDisplay();

        // NOTE: Try GLX_EXT_swap_control first.
        if (hasSwapControlEXT_) {
            POMDOG_CLANG_SUPPRESS_WARNING_PUSH
            POMDOG_CLANG_SUPPRESS_WARNING("-Wcast-function-type-strict")
            const auto glXSwapIntervalEXT = reinterpret_cast<PFNGLXSWAPINTERVALEXTPROC>(
                glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glXSwapIntervalEXT")));
            POMDOG_CLANG_SUPPRESS_WARNING_POP

            if (glXSwapIntervalEXT != nullptr) {
                // NOTE: Calling glXSwapIntervalEXT when the GLX_EXT_swap_control extension
                // is not actually supported causes a Segmentation fault (core dumped).
                // glXGetProcAddressARB() returning a non-null pointer does NOT reliably
                // indicate extension presence; always guard with glXQueryExtensionsString().
                // hasSwapControlEXT_ is set from glXQueryExtensionsString() in initialize().
                glXSwapIntervalEXT(display, glxWindow_, interval);
                return;
            }
        }

        // NOTE: GLX_MESA_swap_control fallback.
        if (hasSwapControlMESA_) {
            POMDOG_CLANG_SUPPRESS_WARNING_PUSH
            POMDOG_CLANG_SUPPRESS_WARNING("-Wcast-function-type-strict")
            const auto glXSwapIntervalMESA = reinterpret_cast<PFNGLXSWAPINTERVALMESAPROC>(
                glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glXSwapIntervalMESA")));
            POMDOG_CLANG_SUPPRESS_WARNING_POP

            if (glXSwapIntervalMESA != nullptr) {
                glXSwapIntervalMESA(static_cast<unsigned int>(interval));
                return;
            }
        }

        // NOTE: GLX_SGI_swap_control fallback (set only; no query).
        if (hasSwapControlSGI_) {
            POMDOG_CLANG_SUPPRESS_WARNING_PUSH
            POMDOG_CLANG_SUPPRESS_WARNING("-Wcast-function-type-strict")
            const auto glXSwapIntervalSGI = reinterpret_cast<PFNGLXSWAPINTERVALSGIPROC>(
                glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glXSwapIntervalSGI")));
            POMDOG_CLANG_SUPPRESS_WARNING_POP

            if (glXSwapIntervalSGI != nullptr) {
                glXSwapIntervalSGI(interval);
                return;
            }
        }

        // NOTE: No swap-control extension is available on this system.
        // V-Sync cannot be controlled at runtime.
    }
};

} // namespace

OpenGLContextX11::OpenGLContextX11() noexcept = default;

OpenGLContextX11::~OpenGLContextX11() noexcept = default;

std::tuple<std::shared_ptr<OpenGLContextX11>, std::unique_ptr<Error>>
OpenGLContextX11::create(
    const std::shared_ptr<GameWindowX11>& window,
    const GLXFBConfig& framebufferConfig) noexcept
{
    auto ctx = std::make_shared<OpenGLContextX11Impl>();
    if (auto err = ctx->initialize(window, framebufferConfig); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(ctx), nullptr);
}

} // namespace pomdog::detail::x11
