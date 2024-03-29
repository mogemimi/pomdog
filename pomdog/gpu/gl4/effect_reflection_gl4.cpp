// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/effect_reflection_gl4.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/gpu/shader_reflections/effect_constant_description.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <sstream>
#include <tuple>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {
namespace {

[[nodiscard]] GLint
getActiveUniformBlockIntValue(const ShaderProgramGL4& shaderProgram, GLuint uniformBlockIndex, GLenum parameterName)
{
    GLint result = 0;
    glGetActiveUniformBlockiv(shaderProgram.value, uniformBlockIndex, parameterName, &result);
    POMDOG_CHECK_ERROR_GL4("glGetActiveUniformBlockiv");

    return result;
}

[[nodiscard]] std::string
getActiveUniformBlockName(const ShaderProgramGL4& shaderProgram, GLuint uniformBlockIndex)
{
    std::string result;

    GLint maxUniformBlockNameLength = getActiveUniformBlockIntValue(shaderProgram, uniformBlockIndex, GL_UNIFORM_BLOCK_NAME_LENGTH);
    std::vector<GLchar> uniformBlockName(maxUniformBlockNameLength + 1, '\0');

    GLsizei uniformBlockNameLength = 0;
    glGetActiveUniformBlockName(
        shaderProgram.value,
        uniformBlockIndex,
        maxUniformBlockNameLength,
        &uniformBlockNameLength,
        uniformBlockName.data());
    POMDOG_CHECK_ERROR_GL4("glGetActiveUniformBlockName");

    result.assign(uniformBlockName.data(), uniformBlockNameLength);

    return result;
}

[[nodiscard]] std::vector<GLuint>
getActiveUniformBlockIndices(const ShaderProgramGL4& shaderProgram, GLuint uniformBlockIndex)
{
    const auto uniformCount = getActiveUniformBlockIntValue(
        shaderProgram, uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS);

    std::vector<GLint> uniformIndices(uniformCount, 0);
    glGetActiveUniformBlockiv(
        shaderProgram.value,
        uniformBlockIndex,
        GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,
        uniformIndices.data());
    POMDOG_CHECK_ERROR_GL4("glGetActiveUniformBlockiv");

    std::vector<GLuint> result(uniformIndices.begin(), uniformIndices.end());
    return result;
}

template <typename Func>
void getActiveUniformsIntValue(
    const ShaderProgramGL4& shaderProgram,
    const std::vector<GLuint>& uniformIndices,
    GLenum parameter,
    Func func)
{
    const auto uniformCount = static_cast<GLsizei>(uniformIndices.size());

    std::vector<GLint> values(uniformIndices.size());

    glGetActiveUniformsiv(
        shaderProgram.value,
        uniformCount,
        uniformIndices.data(),
        parameter,
        values.data());

    for (GLsizei index = 0; index < uniformCount; ++index) {
        func(index, values[index]);
    }
}

[[nodiscard]] std::vector<UniformVariableGL4>
enumerateUniformVariables(const ShaderProgramGL4& shaderProgram, GLuint uniformBlockIndex)
{
    auto const uniformIndices = getActiveUniformBlockIndices(shaderProgram, uniformBlockIndex);

    std::vector<UniformVariableGL4> uniforms(uniformIndices.size());

    getActiveUniformsIntValue(shaderProgram, uniformIndices, GL_UNIFORM_OFFSET,
        [&](GLsizei index, GLint value) { uniforms[index].StartOffset = value; });

    getActiveUniformsIntValue(shaderProgram, uniformIndices, GL_UNIFORM_SIZE,
        [&](GLsizei index, GLint value) { uniforms[index].Elements = value; });

    getActiveUniformsIntValue(shaderProgram, uniformIndices, GL_UNIFORM_TYPE,
        [&](GLsizei index, GLint value) { uniforms[index].Type = value; });

    getActiveUniformsIntValue(shaderProgram, uniformIndices, GL_UNIFORM_ARRAY_STRIDE,
        [&](GLsizei index, GLint value) { uniforms[index].ArrayStride = value; });

    getActiveUniformsIntValue(shaderProgram, uniformIndices, GL_UNIFORM_MATRIX_STRIDE,
        [&](GLsizei index, GLint value) { uniforms[index].MatrixStride = value; });

    getActiveUniformsIntValue(shaderProgram, uniformIndices, GL_UNIFORM_IS_ROW_MAJOR,
        [&](GLsizei index, GLint value) { uniforms[index].IsRowMajor = (value != 0); });

    getActiveUniformsIntValue(shaderProgram, uniformIndices, GL_UNIFORM_NAME_LENGTH, [&](GLsizei index, GLint value) {
        std::vector<GLchar> uniformName(value + 1, '\0');
        GLsizei uniformNameLength = 0;
        glGetActiveUniformName(shaderProgram.value, uniformIndices[index], value, &uniformNameLength, uniformName.data());
        uniforms[index].Name.assign(uniformName.data(), uniformNameLength);
    });

    return uniforms;
}

[[nodiscard]] std::vector<UniformBlockGL4>
enumerateUniformBlocks(const ShaderProgramGL4& shaderProgram)
{
    GLint uniformBlockCount = 0;
    glGetProgramiv(shaderProgram.value, GL_ACTIVE_UNIFORM_BLOCKS, &uniformBlockCount);
    POMDOG_CHECK_ERROR_GL4("glGetProgramiv");

    if (uniformBlockCount <= 0) {
        ///@note This shader has no uniform-block.
        return {};
    }

    std::vector<UniformBlockGL4> uniformBlocks;

    for (GLint index = 0; index < uniformBlockCount; ++index) {
        auto const uniformBlockIndex = static_cast<GLuint>(index);

        UniformBlockGL4 uniformBlock;

        uniformBlock.Name = getActiveUniformBlockName(shaderProgram, uniformBlockIndex);
        uniformBlock.ByteSize = getActiveUniformBlockIntValue(
            shaderProgram, uniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE);
        //uniformBlock.BindingPoint = getActiveUniformBlockIntValue(shaderProgram, uniformBlockIndex, GL_UNIFORM_BLOCK_BINDING);
        uniformBlock.Uniforms = enumerateUniformVariables(shaderProgram, uniformBlockIndex);
        //uniformBlock.BlockIndex = uniformBlockIndex;
        uniformBlock.BlockIndex = glGetUniformBlockIndex(shaderProgram.value, uniformBlock.Name.data());

        uniformBlocks.push_back(std::move(uniformBlock));
    }

    return uniformBlocks;
}

[[nodiscard]] std::vector<UniformGL4>
enumerateUniforms(const ShaderProgramGL4& shaderProgram)
{
    GLint uniformCount = 0;
    glGetProgramiv(shaderProgram.value, GL_ACTIVE_UNIFORMS, &uniformCount);
    POMDOG_CHECK_ERROR_GL4("glGetProgramiv");

    if (uniformCount <= 0) {
        ///@note This shader has no uniform.
        return {};
    }

    GLint maxUniformNameLength = 0;
    glGetProgramiv(shaderProgram.value, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);
    POMDOG_CHECK_ERROR_GL4("glGetProgramiv");

    if (maxUniformNameLength <= 0) {
        return {};
    }

    maxUniformNameLength += 1;
    std::vector<GLchar> name(maxUniformNameLength, '\0');

    std::vector<UniformGL4> uniformVariables;

    for (GLuint uniformIndex = 0; uniformIndex < static_cast<GLuint>(uniformCount); ++uniformIndex) {
        GLint arrayCount = 0;
        GLenum uniformType = 0;
        GLint uniformNameLength = 0;

        glGetActiveUniform(shaderProgram.value, uniformIndex, maxUniformNameLength,
            &uniformNameLength, &arrayCount, &uniformType, name.data());
        POMDOG_CHECK_ERROR_GL4("glGetActiveUniform");

        POMDOG_ASSERT(uniformNameLength > 0);

        auto const location = glGetUniformLocation(shaderProgram.value, name.data());
        POMDOG_CHECK_ERROR_GL4("glGetUniformLocation");

        if (location != -1) {
            ///@note When uniform location is '-1', it is uniform variable in uniform block.
            UniformGL4 uniform;
            uniform.Name.assign(name.data(), uniformNameLength);
            uniform.Type = uniformType;
            uniform.ArrayCount = arrayCount;
            uniform.Location = location;

            POMDOG_ASSERT(
                uniform.Type == GL_SAMPLER_1D ||
                uniform.Type == GL_SAMPLER_2D ||
                uniform.Type == GL_SAMPLER_3D ||
                uniform.Type == GL_SAMPLER_CUBE ||
                uniform.Type == GL_SAMPLER_1D_SHADOW ||
                uniform.Type == GL_SAMPLER_2D_SHADOW ||
                uniform.Type == GL_SAMPLER_1D_ARRAY ||
                uniform.Type == GL_SAMPLER_2D_ARRAY ||
                uniform.Type == GL_SAMPLER_1D_ARRAY_SHADOW ||
                uniform.Type == GL_SAMPLER_2D_ARRAY_SHADOW ||
                uniform.Type == GL_SAMPLER_2D_MULTISAMPLE ||
                uniform.Type == GL_SAMPLER_2D_MULTISAMPLE_ARRAY ||
                uniform.Type == GL_SAMPLER_CUBE_SHADOW ||
                uniform.Type == GL_SAMPLER_BUFFER ||
                uniform.Type == GL_SAMPLER_2D_RECT ||
                uniform.Type == GL_SAMPLER_2D_RECT_SHADOW ||
                uniform.Type == GL_INT_SAMPLER_1D ||
                uniform.Type == GL_INT_SAMPLER_2D ||
                uniform.Type == GL_INT_SAMPLER_3D ||
                uniform.Type == GL_INT_SAMPLER_CUBE ||
                uniform.Type == GL_INT_SAMPLER_1D_ARRAY ||
                uniform.Type == GL_INT_SAMPLER_2D_ARRAY ||
                uniform.Type == GL_INT_SAMPLER_2D_MULTISAMPLE ||
                uniform.Type == GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY ||
                uniform.Type == GL_INT_SAMPLER_BUFFER ||
                uniform.Type == GL_INT_SAMPLER_2D_RECT ||
                uniform.Type == GL_UNSIGNED_INT_SAMPLER_1D ||
                uniform.Type == GL_UNSIGNED_INT_SAMPLER_2D ||
                uniform.Type == GL_UNSIGNED_INT_SAMPLER_3D ||
                uniform.Type == GL_UNSIGNED_INT_SAMPLER_CUBE ||
                uniform.Type == GL_UNSIGNED_INT_SAMPLER_1D_ARRAY ||
                uniform.Type == GL_UNSIGNED_INT_SAMPLER_2D_ARRAY ||
                uniform.Type == GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE ||
                uniform.Type == GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY ||
                uniform.Type == GL_UNSIGNED_INT_SAMPLER_BUFFER ||
                uniform.Type == GL_UNSIGNED_INT_SAMPLER_2D_RECT);

            uniformVariables.push_back(std::move(uniform));
        }
    }

    ///@todo Replace the following code by GL_ARB_shading_language_420pack on OpenGL 4.2+:
    std::sort(std::begin(uniformVariables), std::end(uniformVariables),
        [](const UniformGL4& a, const UniformGL4& b) { return a.Name < b.Name; });

    return uniformVariables;
}

[[nodiscard]] EffectVariableType
toEffectVariableType(GLenum uniformType)
{
    switch (uniformType) {
    case GL_FLOAT:
    case GL_FLOAT_VEC2:
    case GL_FLOAT_VEC3:
    case GL_FLOAT_VEC4:
    case GL_FLOAT_MAT2:
    case GL_FLOAT_MAT3:
    case GL_FLOAT_MAT4:
    case GL_FLOAT_MAT2x3:
    case GL_FLOAT_MAT2x4:
    case GL_FLOAT_MAT3x2:
    case GL_FLOAT_MAT3x4:
    case GL_FLOAT_MAT4x2:
    case GL_FLOAT_MAT4x3:
        return EffectVariableType::Float;

    case GL_INT:
    case GL_INT_VEC2:
    case GL_INT_VEC3:
    case GL_INT_VEC4:
        return EffectVariableType::Int32;

    case GL_UNSIGNED_INT:
    case GL_UNSIGNED_INT_VEC2:
    case GL_UNSIGNED_INT_VEC3:
    case GL_UNSIGNED_INT_VEC4:
        return EffectVariableType::UInt32;

    case GL_DOUBLE:
    case GL_DOUBLE_VEC2:
    case GL_DOUBLE_VEC3:
    case GL_DOUBLE_VEC4:
    case GL_DOUBLE_MAT2:
    case GL_DOUBLE_MAT3:
    case GL_DOUBLE_MAT4:
    case GL_DOUBLE_MAT2x3:
    case GL_DOUBLE_MAT2x4:
    case GL_DOUBLE_MAT3x2:
    case GL_DOUBLE_MAT3x4:
    case GL_DOUBLE_MAT4x2:
    case GL_DOUBLE_MAT4x3:
        return EffectVariableType::Double;

    case GL_BOOL:
    case GL_BOOL_VEC2:
    case GL_BOOL_VEC3:
    case GL_BOOL_VEC4:
        return EffectVariableType::Bool;

    case GL_SAMPLER_1D:
        return EffectVariableType::Texture1D;
    case GL_SAMPLER_2D:
        return EffectVariableType::Texture2D;
    case GL_SAMPLER_3D:
        return EffectVariableType::Texture3D;
    case GL_SAMPLER_CUBE:
        return EffectVariableType::TextureCube;

    case GL_SAMPLER_BUFFER:
        //return EffectVariableType::TextureBuffer; // See also: D3D_SVT_TBUFFER
    case GL_SAMPLER_1D_ARRAY:
        //return EffectVariableType::Texture1DArray; // See also: D3D_SVT_TEXTURE1DARRAY
    case GL_SAMPLER_2D_ARRAY:
        //return EffectVariableType::Texture2DArray; // See also: D3D_SVT_TEXTURE2DARRAY
    case GL_SAMPLER_2D_MULTISAMPLE:
        //return EffectVariableType::Texture2DMultiSample; // See also: D3D_SVT_TEXTURE2DMS
    case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
        //return EffectVariableType::Texture2DMultiSampleArray; // See also: D3D_SVT_TEXTURE2DMSARRAY
    case GL_SAMPLER_1D_SHADOW:
    case GL_SAMPLER_2D_SHADOW:
    case GL_SAMPLER_1D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_ARRAY_SHADOW:
    case GL_SAMPLER_CUBE_SHADOW:
    case GL_SAMPLER_2D_RECT:
    case GL_SAMPLER_2D_RECT_SHADOW:
    case GL_INT_SAMPLER_1D:
    case GL_INT_SAMPLER_2D:
    case GL_INT_SAMPLER_3D:
    case GL_INT_SAMPLER_CUBE:
    case GL_INT_SAMPLER_1D_ARRAY:
    case GL_INT_SAMPLER_2D_ARRAY:
    case GL_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_INT_SAMPLER_BUFFER:
    case GL_INT_SAMPLER_2D_RECT:
    case GL_UNSIGNED_INT_SAMPLER_1D:
    case GL_UNSIGNED_INT_SAMPLER_2D:
    case GL_UNSIGNED_INT_SAMPLER_3D:
    case GL_UNSIGNED_INT_SAMPLER_CUBE:
    case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_BUFFER:
    case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
        // Not supported:
        POMDOG_ASSERT(uniformType);
        break;
    }

    return EffectVariableType::Float;
}

[[nodiscard]] EffectVariableClass
toEffectVariableClass(GLenum uniformType)
{
    switch (uniformType) {
    case GL_FLOAT:
    case GL_DOUBLE:
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_BOOL:
        return EffectVariableClass::Scalar;

    case GL_FLOAT_VEC2:
    case GL_FLOAT_VEC3:
    case GL_FLOAT_VEC4:
    case GL_DOUBLE_VEC2:
    case GL_DOUBLE_VEC3:
    case GL_DOUBLE_VEC4:
    case GL_INT_VEC2:
    case GL_INT_VEC3:
    case GL_INT_VEC4:
    case GL_UNSIGNED_INT_VEC2:
    case GL_UNSIGNED_INT_VEC3:
    case GL_UNSIGNED_INT_VEC4:
    case GL_BOOL_VEC2:
    case GL_BOOL_VEC3:
    case GL_BOOL_VEC4:
        return EffectVariableClass::Vector;

    case GL_FLOAT_MAT2:
    case GL_FLOAT_MAT3:
    case GL_FLOAT_MAT4:
    case GL_FLOAT_MAT2x3:
    case GL_FLOAT_MAT2x4:
    case GL_FLOAT_MAT3x2:
    case GL_FLOAT_MAT3x4:
    case GL_FLOAT_MAT4x2:
    case GL_FLOAT_MAT4x3:
    case GL_DOUBLE_MAT2:
    case GL_DOUBLE_MAT3:
    case GL_DOUBLE_MAT4:
    case GL_DOUBLE_MAT2x3:
    case GL_DOUBLE_MAT2x4:
    case GL_DOUBLE_MAT3x2:
    case GL_DOUBLE_MAT3x4:
    case GL_DOUBLE_MAT4x2:
    case GL_DOUBLE_MAT4x3:
        return EffectVariableClass::Matrix;

    case GL_SAMPLER_1D:
    case GL_SAMPLER_2D:
    case GL_SAMPLER_3D:
    case GL_SAMPLER_CUBE:
    case GL_SAMPLER_BUFFER:
    case GL_SAMPLER_1D_ARRAY:
    case GL_SAMPLER_2D_ARRAY:
    case GL_SAMPLER_2D_MULTISAMPLE:
    case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_SAMPLER_1D_SHADOW:
    case GL_SAMPLER_2D_SHADOW:
    case GL_SAMPLER_1D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_ARRAY_SHADOW:
    case GL_SAMPLER_CUBE_SHADOW:
    case GL_SAMPLER_2D_RECT:
    case GL_SAMPLER_2D_RECT_SHADOW:
    case GL_INT_SAMPLER_1D:
    case GL_INT_SAMPLER_2D:
    case GL_INT_SAMPLER_3D:
    case GL_INT_SAMPLER_CUBE:
    case GL_INT_SAMPLER_1D_ARRAY:
    case GL_INT_SAMPLER_2D_ARRAY:
    case GL_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_INT_SAMPLER_BUFFER:
    case GL_INT_SAMPLER_2D_RECT:
    case GL_UNSIGNED_INT_SAMPLER_1D:
    case GL_UNSIGNED_INT_SAMPLER_2D:
    case GL_UNSIGNED_INT_SAMPLER_3D:
    case GL_UNSIGNED_INT_SAMPLER_CUBE:
    case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_BUFFER:
    case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
        return EffectVariableClass::Object;
    }

    return EffectVariableClass::Struct;
}

// ToComponents returns RowCount and ColumnCount.
[[nodiscard]] std::tuple<int, int>
toComponents(GLenum uniformType)
{
    switch (uniformType) {
    case GL_FLOAT:
    case GL_DOUBLE:
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_BOOL:
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    case GL_SAMPLER_1D:
    case GL_SAMPLER_2D:
    case GL_SAMPLER_3D:
    case GL_SAMPLER_CUBE:
    case GL_SAMPLER_BUFFER:
    case GL_SAMPLER_1D_ARRAY:
    case GL_SAMPLER_2D_ARRAY:
    case GL_SAMPLER_2D_MULTISAMPLE:
    case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_SAMPLER_1D_SHADOW:
    case GL_SAMPLER_2D_SHADOW:
    case GL_SAMPLER_1D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_ARRAY_SHADOW:
    case GL_SAMPLER_CUBE_SHADOW:
    case GL_SAMPLER_2D_RECT:
    case GL_SAMPLER_2D_RECT_SHADOW:
    case GL_INT_SAMPLER_1D:
    case GL_INT_SAMPLER_2D:
    case GL_INT_SAMPLER_3D:
    case GL_INT_SAMPLER_CUBE:
    case GL_INT_SAMPLER_1D_ARRAY:
    case GL_INT_SAMPLER_2D_ARRAY:
    case GL_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_INT_SAMPLER_BUFFER:
    case GL_INT_SAMPLER_2D_RECT:
    case GL_UNSIGNED_INT_SAMPLER_1D:
    case GL_UNSIGNED_INT_SAMPLER_2D:
    case GL_UNSIGNED_INT_SAMPLER_3D:
    case GL_UNSIGNED_INT_SAMPLER_CUBE:
    case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_BUFFER:
    case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
        return std::make_tuple(1, 1);

    case GL_FLOAT_VEC2:
    case GL_INT_VEC2:
    case GL_UNSIGNED_INT_VEC2:
    case GL_DOUBLE_VEC2:
    case GL_BOOL_VEC2:
        return std::make_tuple(1, 2);

    case GL_FLOAT_VEC3:
    case GL_INT_VEC3:
    case GL_UNSIGNED_INT_VEC3:
    case GL_DOUBLE_VEC3:
    case GL_BOOL_VEC3:
        return std::make_tuple(1, 3);

    case GL_FLOAT_VEC4:
    case GL_INT_VEC4:
    case GL_UNSIGNED_INT_VEC4:
    case GL_DOUBLE_VEC4:
    case GL_BOOL_VEC4:
        return std::make_tuple(1, 4);

    case GL_FLOAT_MAT2:
    case GL_DOUBLE_MAT2:
        return std::make_tuple(2, 2);

    case GL_FLOAT_MAT3:
    case GL_DOUBLE_MAT3:
        return std::make_tuple(3, 3);

    case GL_FLOAT_MAT4:
    case GL_DOUBLE_MAT4:
        return std::make_tuple(4, 4);

    case GL_FLOAT_MAT2x3:
    case GL_DOUBLE_MAT2x3:
        return std::make_tuple(2, 3);

    case GL_FLOAT_MAT3x2:
    case GL_DOUBLE_MAT3x2:
        return std::make_tuple(3, 2);

    case GL_FLOAT_MAT2x4:
    case GL_DOUBLE_MAT2x4:
        return std::make_tuple(2, 4);

    case GL_FLOAT_MAT4x2:
    case GL_DOUBLE_MAT4x2:
        return std::make_tuple(4, 2);

    case GL_FLOAT_MAT3x4:
    case GL_DOUBLE_MAT3x4:
        return std::make_tuple(3, 4);

    case GL_FLOAT_MAT4x3:
    case GL_DOUBLE_MAT4x3:
        return std::make_tuple(4, 3);

    default:
        break;
    }

    return std::make_tuple(1, 1);
}

[[nodiscard]] EffectAnnotation
toEffectAnnotation(const UniformVariableGL4& uniform)
{
    EffectAnnotation annotation;
    annotation.VariableType = toEffectVariableType(uniform.Type);
    annotation.VariableClass = toEffectVariableClass(uniform.Type);
    const auto [rowCount, columnCount] = toComponents(uniform.Type);
    annotation.RowCount = static_cast<std::uint8_t>(rowCount);
    annotation.ColumnCount = static_cast<std::uint8_t>(columnCount);
    annotation.Elements = (uniform.Elements > 1) ? static_cast<decltype(annotation.Elements)>(uniform.Elements) : 0;
    POMDOG_ASSERT(annotation.Elements != 1);
    return annotation;
}

[[nodiscard]] std::vector<EffectVariable>
getEffectVariables(const std::vector<UniformVariableGL4>& uniforms)
{
    std::vector<EffectVariable> result;
    result.reserve(uniforms.size());

    for (auto& uniform : uniforms) {
        EffectVariable effectVariable;
        effectVariable.Name = uniform.Name;
        effectVariable.StartOffset = uniform.StartOffset;
        effectVariable.Annotation = toEffectAnnotation(uniform);
        result.push_back(std::move(effectVariable));
    }
    return result;
}

} // namespace

std::unique_ptr<Error>
EffectReflectionGL4::initialize(const ShaderProgramGL4& shaderProgramIn) noexcept
{
    shaderProgram_ = shaderProgramIn;
    return nullptr;
}

std::vector<UniformBlockGL4>
EffectReflectionGL4::getNativeUniformBlocks()
{
    auto uniformBlocks = enumerateUniformBlocks(shaderProgram_);
    return uniformBlocks;
}

std::vector<UniformGL4>
EffectReflectionGL4::getNativeUniforms()
{
    auto uniforms = enumerateUniforms(shaderProgram_);
    return uniforms;
}

std::vector<EffectConstantDescription>
EffectReflectionGL4::getConstantBuffers() const noexcept
{
    auto uniformBlocks = enumerateUniformBlocks(shaderProgram_);

    std::vector<EffectConstantDescription> result;

    for (auto& uniformBlock : uniformBlocks) {
        EffectConstantDescription description;
        description.Name = uniformBlock.Name;
        description.ByteSize = uniformBlock.ByteSize;
        description.Variables = getEffectVariables(uniformBlock.Uniforms);
        result.push_back(std::move(description));
    }

    return result;
}

} // namespace pomdog::gpu::detail::gl4
