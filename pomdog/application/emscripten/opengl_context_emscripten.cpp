// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/emscripten/opengl_context_emscripten.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten/html5_webgl.h>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::emscripten {
namespace {

class OpenGLContextEmscriptenImpl final : public OpenGLContextEmscripten {
private:
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webGLContext_ = 0;

public:
    OpenGLContextEmscriptenImpl() noexcept = default;

    OpenGLContextEmscriptenImpl(const OpenGLContextEmscriptenImpl&) = delete;
    OpenGLContextEmscriptenImpl& operator=(const OpenGLContextEmscriptenImpl&) = delete;

    ~OpenGLContextEmscriptenImpl() noexcept override
    {
        if (webGLContext_ != 0) {
            emscripten_webgl_destroy_context(webGLContext_);
        }
    }

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::string& targetCanvas,
        [[maybe_unused]] const gpu::PresentationParameters& presentationParameters) noexcept
    {
        EmscriptenWebGLContextAttributes attrs = {};
        emscripten_webgl_init_context_attributes(&attrs);
        attrs.alpha = EM_FALSE;
        attrs.depth = EM_TRUE;
        attrs.stencil = EM_TRUE;
        attrs.antialias = EM_FALSE;

        attrs.premultipliedAlpha = EM_TRUE;
        attrs.preserveDrawingBuffer = EM_TRUE;

        attrs.powerPreference = EM_WEBGL_POWER_PREFERENCE_DEFAULT;
        attrs.enableExtensionsByDefault = EM_TRUE;

        // NOTE: with "-s OFFSCREEN_FRAMEBUFFER=1"
        attrs.explicitSwapControl = EM_TRUE;
        attrs.renderViaOffscreenBackBuffer = 1;

        // NOTE: WebGL 2.0
        attrs.majorVersion = 2;
        attrs.minorVersion = 0;

        if (targetCanvas.empty()) {
            return errors::make("target canvas is empty.");
        }

        webGLContext_ = emscripten_webgl_create_context(targetCanvas.c_str(), &attrs);
        if (webGLContext_ == 0) {
            return errors::make("emscripten_webgl_create_context() failed.");
        }

        if (auto result = emscripten_webgl_make_context_current(webGLContext_);
            result != EMSCRIPTEN_RESULT_SUCCESS) {
            return errors::make("emscripten_webgl_make_context_current() failed.");
        }

        return nullptr;
    }

    void makeCurrent() override
    {
        emscripten_webgl_make_context_current(webGLContext_);
    }

    void clearCurrent() override
    {
        emscripten_webgl_make_context_current(0);
    }

    void swapBuffers() override
    {
        emscripten_webgl_commit_frame();
    }
};

} // namespace

OpenGLContextEmscripten::OpenGLContextEmscripten() noexcept = default;

OpenGLContextEmscripten::~OpenGLContextEmscripten() noexcept = default;

std::tuple<std::shared_ptr<OpenGLContextEmscripten>, std::unique_ptr<Error>>
OpenGLContextEmscripten::create(
    const std::string& targetCanvas,
    const gpu::PresentationParameters& presentationParameters) noexcept
{
    auto ctx = std::make_shared<OpenGLContextEmscriptenImpl>();
    if (auto err = ctx->initialize(targetCanvas, presentationParameters); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(ctx), nullptr);
}

} // namespace pomdog::detail::emscripten
