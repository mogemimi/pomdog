// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/platform/emscripten/opengl_context_emscripten.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::emscripten {

OpenGLContextEmscripten::OpenGLContextEmscripten() noexcept = default;

OpenGLContextEmscripten::~OpenGLContextEmscripten() noexcept
{
    if (webGLContext_ != 0) {
        emscripten_webgl_destroy_context(webGLContext_);
    }
}

std::unique_ptr<Error>
OpenGLContextEmscripten::initialize(
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

void OpenGLContextEmscripten::makeCurrent()
{
    emscripten_webgl_make_context_current(webGLContext_);
}

void OpenGLContextEmscripten::clearCurrent()
{
    emscripten_webgl_make_context_current(0);
}

void OpenGLContextEmscripten::swapBuffers()
{
    emscripten_webgl_commit_frame();
}

} // namespace pomdog::detail::emscripten
