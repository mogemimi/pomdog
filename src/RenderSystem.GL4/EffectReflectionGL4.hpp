// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "TypesafeGL4.hpp"
#include "Pomdog/Graphics/EffectReflection.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace Pomdog::Detail::GL4 {

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
    explicit EffectReflectionGL4(const ShaderProgramGL4& shaderProgram);

    std::vector<EffectConstantDescription>
    GetConstantBuffers() const noexcept override;

    std::vector<UniformBlockGL4> GetNativeUniformBlocks();

    std::vector<UniformGL4> GetNativeUniforms();

private:
    ShaderProgramGL4 shaderProgram;
};

} // namespace Pomdog::Detail::GL4
