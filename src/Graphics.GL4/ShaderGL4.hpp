// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {
class ShaderBytecode;
} // namespace Pomdog::Detail

namespace Pomdog::Detail::GL4 {

template <GLenum PipelineStage>
class ShaderGL4 final : public Shader {
public:
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(const ShaderBytecode& source) noexcept;

    ~ShaderGL4();

    GLuint GetShader() const;

private:
    static constexpr GLenum pipelineStage = PipelineStage;
    static_assert(pipelineStage == GL_VERTEX_SHADER
        || pipelineStage == GL_FRAGMENT_SHADER
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
        , "");

    std::optional<GLuint> shader;
};

using VertexShaderGL4 = ShaderGL4<GL_VERTEX_SHADER>;
using PixelShaderGL4 = ShaderGL4<GL_FRAGMENT_SHADER>;

} // namespace Pomdog::Detail::GL4
