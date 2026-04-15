// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/gl4/opengl_context.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten/html5_webgl.h>
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::gpu {
struct PresentationParameters;
} // namespace pomdog::gpu

namespace pomdog::detail::emscripten {

class OpenGLContextEmscripten final : public gpu::detail::gl4::OpenGLContext {
private:
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webGLContext_ = 0;

public:
    OpenGLContextEmscripten() noexcept;

    OpenGLContextEmscripten(const OpenGLContextEmscripten&) = delete;
    OpenGLContextEmscripten& operator=(const OpenGLContextEmscripten&) = delete;

    ~OpenGLContextEmscripten() noexcept override;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::string& targetCanvas,
        const gpu::PresentationParameters& presentationParameters) noexcept;

    void makeCurrent() override;

    void clearCurrent() override;

    void swapBuffers() override;
};

} // namespace pomdog::detail::emscripten
