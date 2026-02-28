// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/gpu/gl4/typesafe_gl4.h"
#include "pomdog/gpu/shader_reflections/effect_reflection.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {

struct UniformVariableGL4 final {
    std::string name;
    GLuint startOffset;
    GLenum elements;
    GLenum type;
    GLuint arrayStride;
    GLuint matrixStride;
    bool isRowMajor;
};

struct UniformBlockGL4 final {
    std::vector<UniformVariableGL4> uniforms;
    std::string name;
    u32 byteSize;
    u32 blockIndex;
};

struct UniformGL4 final {
    std::string name;
    GLint location;
    GLenum type;
    GLuint arrayCount;
};

class EffectReflectionGL4 final : public EffectReflection {
private:
    ShaderProgramGL4 shaderProgram_ = {};

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(const ShaderProgramGL4& shaderProgram) noexcept;

    [[nodiscard]] std::vector<EffectConstantDescription>
    getConstantBuffers() const noexcept override;

    [[nodiscard]] std::vector<UniformBlockGL4>
    getNativeUniformBlocks();

    [[nodiscard]] std::vector<UniformGL4>
    getNativeUniforms();
};

} // namespace pomdog::gpu::detail::gl4
