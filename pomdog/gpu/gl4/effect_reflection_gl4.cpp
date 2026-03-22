// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/effect_reflection_gl4.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {
namespace {

[[nodiscard]] GLint
getActiveUniformBlockIntValue(const ShaderProgramGL4& shaderProgram, GLuint uniformBlockIndex, GLenum parameterName) noexcept
{
    GLint result = 0;
    glGetActiveUniformBlockiv(shaderProgram.value, uniformBlockIndex, parameterName, &result);
    POMDOG_CHECK_ERROR_GL4("glGetActiveUniformBlockiv");
    return result;
}

[[nodiscard]] std::string
getActiveUniformBlockName(const ShaderProgramGL4& shaderProgram, GLuint uniformBlockIndex) noexcept
{
    const auto maxUniformBlockNameLength = getActiveUniformBlockIntValue(
        shaderProgram, uniformBlockIndex, GL_UNIFORM_BLOCK_NAME_LENGTH);

    if (maxUniformBlockNameLength <= 0) {
        return {};
    }

    std::string result;
    result.resize(static_cast<std::size_t>(maxUniformBlockNameLength));

    GLsizei uniformBlockNameLength = 0;
    glGetActiveUniformBlockName(
        shaderProgram.value,
        uniformBlockIndex,
        maxUniformBlockNameLength,
        &uniformBlockNameLength,
        result.data());
    POMDOG_CHECK_ERROR_GL4("glGetActiveUniformBlockName");

    if (uniformBlockNameLength > 0) {
        result.resize(static_cast<std::size_t>(uniformBlockNameLength));
    }
    else {
        result.clear();
    }

    return result;
}

} // namespace

std::vector<UniformBlockGL4>
enumerateUniformBlocks(const ShaderProgramGL4& shaderProgram) noexcept
{
    GLint uniformBlockCount = 0;
    glGetProgramiv(shaderProgram.value, GL_ACTIVE_UNIFORM_BLOCKS, &uniformBlockCount);
    POMDOG_CHECK_ERROR_GL4("glGetProgramiv");

    if (uniformBlockCount <= 0) {
        return {};
    }

    std::vector<UniformBlockGL4> uniformBlocks;
    uniformBlocks.reserve(static_cast<std::size_t>(uniformBlockCount));

    for (GLint index = 0; index < uniformBlockCount; ++index) {
        const auto uniformBlockIndex = static_cast<GLuint>(index);

        UniformBlockGL4 uniformBlock;
        uniformBlock.name = getActiveUniformBlockName(shaderProgram, uniformBlockIndex);

        if (const auto v = getActiveUniformBlockIntValue(shaderProgram, uniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE); v >= 0) {
            uniformBlock.byteSize = static_cast<u32>(v);
        }
        else {
            uniformBlock.byteSize = 0;
        }

        uniformBlock.blockIndex = glGetUniformBlockIndex(shaderProgram.value, uniformBlock.name.data());
        POMDOG_CHECK_ERROR_GL4("glGetUniformBlockIndex");

        uniformBlocks.push_back(std::move(uniformBlock));
    }

    return uniformBlocks;
}

std::vector<UniformGL4>
enumerateUniforms(const ShaderProgramGL4& shaderProgram) noexcept
{
    GLint uniformCount = 0;
    glGetProgramiv(shaderProgram.value, GL_ACTIVE_UNIFORMS, &uniformCount);
    POMDOG_CHECK_ERROR_GL4("glGetProgramiv");

    if (uniformCount <= 0) {
        return {};
    }

    GLint maxUniformNameLength = 0;
    glGetProgramiv(shaderProgram.value, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);
    POMDOG_CHECK_ERROR_GL4("glGetProgramiv");

    if (maxUniformNameLength <= 0) {
        return {};
    }

    maxUniformNameLength += 1;
    std::vector<GLchar> name(static_cast<std::size_t>(maxUniformNameLength), '\0');

    std::vector<UniformGL4> uniformVariables;

    for (GLuint uniformIndex = 0; uniformIndex < static_cast<GLuint>(uniformCount); ++uniformIndex) {
        GLint arrayCount = 0;
        GLenum uniformType = 0;
        GLint uniformNameLength = 0;

        glGetActiveUniform(shaderProgram.value, uniformIndex, maxUniformNameLength,
            &uniformNameLength, &arrayCount, &uniformType, name.data());
        POMDOG_CHECK_ERROR_GL4("glGetActiveUniform");

        POMDOG_ASSERT(uniformNameLength > 0);

        const auto location = glGetUniformLocation(shaderProgram.value, name.data());
        POMDOG_CHECK_ERROR_GL4("glGetUniformLocation");

        if (location != -1) {
            // NOTE: When uniform location is '-1', it is a uniform variable in a uniform block.
            UniformGL4 uniform;
            uniform.name.assign(name.data(), static_cast<std::size_t>(uniformNameLength));
            uniform.type = uniformType;
            uniform.arrayCount = static_cast<GLuint>(arrayCount);
            uniform.location = location;
            uniformVariables.push_back(std::move(uniform));
        }
    }

    // TODO: Replace the following code by GL_ARB_shading_language_420pack on OpenGL 4.2+:
    std::sort(std::begin(uniformVariables), std::end(uniformVariables),
        [](const UniformGL4& a, const UniformGL4& b) { return a.name < b.name; });

    return uniformVariables;
}

} // namespace pomdog::gpu::detail::gl4
