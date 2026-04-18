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
isExtensionSupported(const char* extensionList, const char* extension) noexcept
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

class OpenGLContextX11Impl final : public OpenGLContextX11 {
private:
    std::shared_ptr<GameWindowX11> window_;
    ::GLXContext glxContext_ = nullptr;
    bool isOpenGL3Supported_ = false;

public:
    ~OpenGLContextX11Impl() noexcept override
    {
        if (glxContext_ != nullptr) {
            POMDOG_ASSERT(window_ != nullptr);
            const auto display = window_->getNativeDisplay();

            if (glXGetCurrentContext() == glxContext_) {
                glXMakeCurrent(display, None, nullptr);
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

        return nullptr;
    }

    void
    makeCurrent() override
    {
        if (glXGetCurrentContext() == glxContext_) {
            return;
        }

        POMDOG_ASSERT(window_ != nullptr);
        glXMakeCurrent(window_->getNativeDisplay(), window_->getNativeWindow(), glxContext_);
    }

    void
    clearCurrent() override
    {
        POMDOG_ASSERT(window_ != nullptr);
        glXMakeCurrent(window_->getNativeDisplay(), None, nullptr);
    }

    void
    swapBuffers() override
    {
        POMDOG_ASSERT(window_ != nullptr);
        glXSwapBuffers(window_->getNativeDisplay(), window_->getNativeWindow());
    }

    bool
    isOpenGL3Supported() const noexcept override
    {
        return isOpenGL3Supported_;
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
