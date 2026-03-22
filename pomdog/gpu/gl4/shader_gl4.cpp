// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/shader_gl4.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/gpu/backends/shader_bytecode.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/string_hash32.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/shader_reflect_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstring>
#include <limits>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {
namespace {

[[nodiscard]] std::tuple<std::optional<GLuint>, std::unique_ptr<Error>>
compileShader(const ShaderBytecode& source, GLenum pipelineStage) noexcept
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    {
        const auto validPipelineStage = (pipelineStage == GL_VERTEX_SHADER)
#ifdef GL_FRAGMENT_SHADER
                                        || (pipelineStage == GL_FRAGMENT_SHADER)
#endif
#ifdef GL_GEOMETRY_SHADER
                                        || (pipelineStage == GL_GEOMETRY_SHADER)
#endif
#ifdef GL_TESS_CONTROL_SHADER
                                        || (pipelineStage == GL_TESS_CONTROL_SHADER)
#endif
#ifdef GL_TESS_EVALUATION_SHADER
                                        || (pipelineStage == GL_TESS_EVALUATION_SHADER)
#endif
#ifdef GL_COMPUTE_SHADER
                                        || (pipelineStage == GL_COMPUTE_SHADER)
#endif
            ;
        POMDOG_ASSERT(validPipelineStage);
    }
#endif

    POMDOG_ASSERT(source.code != nullptr);
    POMDOG_ASSERT(source.byteLength > 0);

    auto result = std::make_optional(glCreateShader(pipelineStage));
    if (*result == 0) {
        return std::make_tuple(std::nullopt, errors::make("glCreateShader() failed"));
    }

    std::array<const GLchar*, 1> shaderSource = {{
        reinterpret_cast<const GLchar*>(source.code),
    }};

    POMDOG_ASSERT(source.byteLength < static_cast<decltype(source.byteLength)>(std::numeric_limits<GLint>::max()));
    GLint const sourceLength = static_cast<GLint>(source.byteLength);

    glShaderSource(*result, 1, shaderSource.data(), &sourceLength);

    glCompileShader(*result);
    POMDOG_CHECK_ERROR_GL4("glCompileShader");

    GLint compileSuccess = 0;
    glGetShaderiv(*result, GL_COMPILE_STATUS, &compileSuccess);

    if (compileSuccess == GL_FALSE) {
        std::array<GLchar, 256> messageBuffer;

        glGetShaderInfoLog(*result, static_cast<GLsizei>(messageBuffer.size()), nullptr, messageBuffer.data());
        messageBuffer.back() = '\0';
        const std::string message = messageBuffer.data();

        glDeleteShader(*result);
        return std::make_tuple(std::nullopt, errors::make("glCompileShader() failed: " + message));
    }

    return std::make_tuple(std::move(result), nullptr);
}

} // namespace

template <GLenum PipelineStage>
std::unique_ptr<Error>
ShaderGL4<PipelineStage>::initialize(const ShaderBytecode& source) noexcept
{
    auto [result, compileErr] = compileShader(source, pipelineStage);
    if (compileErr != nullptr) {
        return errors::wrap(std::move(compileErr), "failed to compile shader");
    }
    POMDOG_ASSERT(result != std::nullopt);
    shader_ = std::move(result);

    if (source.reflectionData != nullptr && source.reflectionByteLength > 0) {
        reflectionData_ = std::make_unique<u8[]>(source.reflectionByteLength);
        std::memcpy(reflectionData_.get(), source.reflectionData, source.reflectionByteLength);
        reflectionByteLength_ = source.reflectionByteLength;
    }

    return nullptr;
}

template <GLenum PipelineStage>
ShaderGL4<PipelineStage>::~ShaderGL4()
{
    if (shader_ != std::nullopt) {
        glDeleteShader(*shader_);
        shader_ = std::nullopt;
    }
}

template <GLenum PipelineStage>
GLuint ShaderGL4<PipelineStage>::getShader() const
{
    POMDOG_ASSERT(shader_ != std::nullopt);
    return *shader_;
}

template <GLenum PipelineStage>
std::optional<u8>
ShaderGL4<PipelineStage>::findConstantBufferSlotIndex(std::string_view name) const noexcept
{
    if (reflectionData_ == nullptr || reflectionByteLength_ == 0) {
        return std::nullopt;
    }
    auto reflect = pomdogschemas::GetShaderReflect(reflectionData_.get());
    if (reflect->cbuffers() == nullptr) {
        return std::nullopt;
    }
    const auto nameHash = computeStringHash32(name);
    if (auto found = reflect->cbuffers()->LookupByKey(nameHash); found != nullptr) {
        return found->slot();
    }
    return std::nullopt;
}

template <GLenum PipelineStage>
std::optional<u8>
ShaderGL4<PipelineStage>::findSamplerSlotIndex(std::string_view name) const noexcept
{
    if (reflectionData_ == nullptr || reflectionByteLength_ == 0) {
        return std::nullopt;
    }
    auto reflect = pomdogschemas::GetShaderReflect(reflectionData_.get());
    if (reflect->samplers() == nullptr) {
        return std::nullopt;
    }
    const auto nameHash = computeStringHash32(name);
    if (auto found = reflect->samplers()->LookupByKey(nameHash); found != nullptr) {
        return found->slot();
    }
    return std::nullopt;
}

// explicit instantiations
template class ShaderGL4<GL_VERTEX_SHADER>;
template class ShaderGL4<GL_FRAGMENT_SHADER>;
//template class ShaderGL4<GL_GEOMETRY_SHADER>;
//template class ShaderGL4<GL_TESS_CONTROL_SHADER>;
//template class ShaderGL4<GL_TESS_EVALUATION_SHADER>;
//template class ShaderGL4<GL_COMPUTE_SHADER>;

} // namespace pomdog::gpu::detail::gl4
