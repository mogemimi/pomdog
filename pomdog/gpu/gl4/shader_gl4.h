// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <span>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {

/// ShaderGL4 is the OpenGL 4 implementation of Shader for a given pipeline stage.
template <GLenum PipelineStage>
class ShaderGL4 final : public Shader {
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

    std::optional<GLuint> shader_;
    std::unique_ptr<u8[]> reflectionData_;

public:
    /// Compiles the GLSL source and loads reflection data from the blob.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(std::span<const u8> source, std::span<const u8> reflectionBlob) noexcept;

    ~ShaderGL4() override;

    /// Returns the GL shader object handle.
    [[nodiscard]] GLuint
    getShader() const;

    /// Looks up the constant buffer slot index by name (xxHash32 key).
    [[nodiscard]] std::optional<u8>
    findConstantBufferSlotIndex(std::string_view name) const noexcept;

    /// Looks up the sampler slot index by name (xxHash32 key).
    [[nodiscard]] std::optional<u8>
    findSamplerSlotIndex(std::string_view name) const noexcept;
};

using VertexShaderGL4 = ShaderGL4<GL_VERTEX_SHADER>;
using PixelShaderGL4 = ShaderGL4<GL_FRAGMENT_SHADER>;

} // namespace pomdog::gpu::detail::gl4
