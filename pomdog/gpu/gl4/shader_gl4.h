// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail {
class ShaderBytecode;
} // namespace pomdog::gpu::detail

namespace pomdog::gpu::detail::gl4 {

template <GLenum PipelineStage>
class ShaderGL4 final : public Shader {
public:
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(const ShaderBytecode& source) noexcept;

    ~ShaderGL4() override;

    GLuint GetShader() const;

private:
    static constexpr GLenum pipelineStage = PipelineStage;
    static_assert(pipelineStage == GL_VERTEX_SHADER
#ifdef GL_FRAGMENT_SHADER
                  || pipelineStage == GL_FRAGMENT_SHADER
#endif
#ifdef GL_GEOMETRY_SHADER
                  || pipelineStage == GL_GEOMETRY_SHADER
#endif
#ifdef GL_TESS_CONTROL_SHADER
                  || pipelineStage == GL_TESS_CONTROL_SHADER
#endif
#ifdef GL_TESS_EVALUATION_SHADER
                  || pipelineStage == GL_TESS_EVALUATION_SHADER
#endif
#ifdef GL_COMPUTE_SHADER
                  || pipelineStage == GL_COMPUTE_SHADER
#endif
    );

    std::optional<GLuint> shader;
};

using VertexShaderGL4 = ShaderGL4<GL_VERTEX_SHADER>;
using PixelShaderGL4 = ShaderGL4<GL_FRAGMENT_SHADER>;

} // namespace pomdog::gpu::detail::gl4
