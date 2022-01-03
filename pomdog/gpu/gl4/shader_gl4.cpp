// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/shader_gl4.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/shader_bytecode.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <limits>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {
namespace {

[[nodiscard]] std::tuple<std::optional<GLuint>, std::unique_ptr<Error>>
CompileShader(const ShaderBytecode& source, GLenum pipelineStage) noexcept
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

    POMDOG_ASSERT(source.Code != nullptr);
    POMDOG_ASSERT(source.ByteLength > 0);

    auto result = std::make_optional(glCreateShader(pipelineStage));
    if (*result == 0) {
        return std::make_tuple(std::nullopt, errors::New("glCreateShader() failed"));
    }

    std::array<const GLchar*, 1> shaderSource = {{
        reinterpret_cast<const GLchar*>(source.Code),
    }};

    POMDOG_ASSERT(source.ByteLength < static_cast<decltype(source.ByteLength)>(std::numeric_limits<GLint>::max()));
    GLint const sourceLength = static_cast<GLint>(source.ByteLength);

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
        return std::make_tuple(std::nullopt, errors::New("glCompileShader() failed: " + message));
    }

    return std::make_tuple(std::move(result), nullptr);
}

} // namespace

template <GLenum PipelineStage>
std::unique_ptr<Error>
ShaderGL4<PipelineStage>::Initialize(const ShaderBytecode& source) noexcept
{
    auto [result, compileErr] = CompileShader(source, pipelineStage);
    if (compileErr != nullptr) {
        return errors::Wrap(std::move(compileErr), "failed to compile shader");
    }
    POMDOG_ASSERT(result != std::nullopt);
    shader = std::move(result);

    return nullptr;
}

template <GLenum PipelineStage>
ShaderGL4<PipelineStage>::~ShaderGL4()
{
    if (shader != std::nullopt) {
        glDeleteShader(*shader);
        shader = std::nullopt;
    }
}

template <GLenum PipelineStage>
GLuint ShaderGL4<PipelineStage>::GetShader() const
{
    POMDOG_ASSERT(shader != std::nullopt);
    return *shader;
}

// explicit instantiations
template class ShaderGL4<GL_VERTEX_SHADER>;
template class ShaderGL4<GL_FRAGMENT_SHADER>;
//template class ShaderGL4<GL_GEOMETRY_SHADER>;
//template class ShaderGL4<GL_TESS_CONTROL_SHADER>;
//template class ShaderGL4<GL_TESS_EVALUATION_SHADER>;
//template class ShaderGL4<GL_COMPUTE_SHADER>;

} // namespace pomdog::gpu::detail::gl4
