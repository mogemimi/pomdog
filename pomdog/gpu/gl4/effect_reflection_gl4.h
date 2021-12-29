// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/effect_reflection.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/gpu/gl4/typesafe_gl4.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {

struct UniformVariableGL4 final {
    std::string Name;
    GLuint StartOffset;
    GLenum Elements;
    GLenum Type;
    GLuint ArrayStride;
    GLuint MatrixStride;
    bool IsRowMajor;
};

struct UniformBlockGL4 final {
    std::vector<UniformVariableGL4> Uniforms;
    std::string Name;
    std::uint32_t ByteSize;
    std::uint32_t BlockIndex;
};

struct UniformGL4 final {
    std::string Name;
    GLint Location;
    GLenum Type;
    GLuint ArrayCount;
};

class EffectReflectionGL4 final : public EffectReflection {
public:
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(const ShaderProgramGL4& shaderProgram) noexcept;

    std::vector<EffectConstantDescription>
    GetConstantBuffers() const noexcept override;

    std::vector<UniformBlockGL4> GetNativeUniformBlocks();

    std::vector<UniformGL4> GetNativeUniforms();

private:
    ShaderProgramGL4 shaderProgram;
};

} // namespace pomdog::gpu::detail::gl4
