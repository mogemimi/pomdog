// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/gl4/opengl_context.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::gpu {
struct PresentationParameters;
} // namespace pomdog::gpu

namespace pomdog::detail::emscripten {

class OpenGLContextEmscripten : public gpu::detail::gl4::OpenGLContext {
public:
    OpenGLContextEmscripten() noexcept;

    ~OpenGLContextEmscripten() noexcept override;

    [[nodiscard]] static std::tuple<std::shared_ptr<OpenGLContextEmscripten>, std::unique_ptr<Error>>
    create(
        const std::string& targetCanvas,
        const gpu::PresentationParameters& presentationParameters) noexcept;
};

} // namespace pomdog::detail::emscripten
