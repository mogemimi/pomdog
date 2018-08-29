// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "ShaderGL4.hpp"
#include "ErrorChecker.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Logging/LogLevel.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#include <array>

namespace Pomdog {
namespace Detail {
namespace GL4 {
namespace {

std::optional<GLuint> CompileShader(const ShaderBytecode& source, GLenum pipelineStage)
{
#if defined(DEBUG)
    {
        auto validPipelineStage = (pipelineStage == GL_VERTEX_SHADER)
            || (pipelineStage == GL_FRAGMENT_SHADER)
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
        // error
        return std::nullopt;
    }

    std::array<const GLchar*, 1> shaderSource = {{
        reinterpret_cast<const GLchar*>(source.Code)
    }};

    POMDOG_ASSERT(source.ByteLength < static_cast<decltype(source.ByteLength)>(std::numeric_limits<GLint>::max()));
    GLint const sourceLength = static_cast<GLint>(source.ByteLength);

    glShaderSource(*result, 1, shaderSource.data(), &sourceLength);

    glCompileShader(*result);
    POMDOG_CHECK_ERROR_GL4("glCompileShader");

    GLint compileSuccess = 0;
    glGetShaderiv(*result, GL_COMPILE_STATUS, &compileSuccess);

    if (compileSuccess == GL_FALSE) {
#ifdef DEBUG
        std::array<GLchar, 256> messageBuffer;

        glGetShaderInfoLog(*result, messageBuffer.size(), 0, messageBuffer.data());
        std::string const message = messageBuffer.data();

        Log::Critical("Pomdog.RenderSystem", StringHelper::Format(
            "Failed to compile shader.\nerror: %s", message.c_str()));
#endif // defined(DEBUG)

        glDeleteShader(*result);
        return std::nullopt;
    }

    return result;
}

} // unnamed namespace

template <GLenum PipelineStage>
constexpr GLenum ShaderGL4<PipelineStage>::pipelineStage;

template <GLenum PipelineStage>
ShaderGL4<PipelineStage>::ShaderGL4(const ShaderBytecode& source)
{
    shader = CompileShader(source, pipelineStage);
    if (!shader) {
        POMDOG_THROW_EXCEPTION(std::domain_error, "Failed to compile shader");
    }
}

template <GLenum PipelineStage>
ShaderGL4<PipelineStage>::~ShaderGL4()
{
    if (shader) {
        glDeleteShader(*shader);
        shader = std::nullopt;
    }
}

template <GLenum PipelineStage>
GLuint ShaderGL4<PipelineStage>::GetShader() const
{
    POMDOG_ASSERT(shader);
    return *shader;
}

// explicit instantiations
template class ShaderGL4<GL_VERTEX_SHADER>;
template class ShaderGL4<GL_FRAGMENT_SHADER>;
//template class ShaderGL4<GL_GEOMETRY_SHADER>;
//template class ShaderGL4<GL_TESS_CONTROL_SHADER>;
//template class ShaderGL4<GL_TESS_EVALUATION_SHADER>;
//template class ShaderGL4<GL_COMPUTE_SHADER>;

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
