//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "ShaderGL4.hpp"
#include "ErrorChecker.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Logging/LogLevel.hpp"
#include "Pomdog/Logging/LogStream.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <array>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {
namespace {

static Optional<GLuint> CompileShader(ShaderBytecode const& source, GLenum pipelineStage)
{
#if defined(DEBUG)
	{
		auto validPipelineStage = pipelineStage == GL_VERTEX_SHADER
			|| pipelineStage == GL_FRAGMENT_SHADER
			|| pipelineStage == GL_GEOMETRY_SHADER
			|| pipelineStage == GL_TESS_CONTROL_SHADER
			|| pipelineStage == GL_TESS_EVALUATION_SHADER
		#ifdef GL_COMPUTE_SHADER
			|| pipelineStage == GL_COMPUTE_SHADER
		#endif
			;
		POMDOG_ASSERT(validPipelineStage);
	}
#endif

	auto result = MakeOptional<GLuint>(glCreateShader(pipelineStage));

	if (result.value() == 0) {
		// error
		return OptionalType::NullOptional;
	}

	std::array<GLchar const*, 1> shaderSource = {{
		reinterpret_cast<GLchar const*>(source.Code)
	}};

	POMDOG_ASSERT(source.ByteLength < static_cast<decltype(source.ByteLength)>(std::numeric_limits<GLint>::max()));
	GLint const sourceLength = static_cast<GLint>(source.ByteLength);

	glShaderSource(result.value(), 1, shaderSource.data(), &sourceLength);

	glCompileShader(result.value());

	#ifdef DEBUG
	ErrorChecker::CheckError("glCompileShader", __FILE__, __LINE__);
	#endif

	GLint compileSuccess = 0;
	glGetShaderiv(result.value(), GL_COMPILE_STATUS, &compileSuccess);

	if (compileSuccess == GL_FALSE)
	{
#ifdef DEBUG
		std::array<GLchar, 256> messageBuffer;

		glGetShaderInfoLog(result.value(), messageBuffer.size(), 0, messageBuffer.data());
		std::string const message = messageBuffer.data();

		Log::Stream(LogLevel::Critical)
			<< "Failed to compile shader.\nerror: " << message;
#endif // defined(DEBUG)

		glDeleteShader(result.value());
		return OptionalType::NullOptional;
	}

	return result;
}

}// unnamed namespace
//-----------------------------------------------------------------------
template <GLenum PipelineStage>
constexpr GLenum ShaderGL4<PipelineStage>::pipelineStage;
//-----------------------------------------------------------------------
template <GLenum PipelineStage>
ShaderGL4<PipelineStage>::ShaderGL4(ShaderBytecode const& source)
{
	shader = CompileShader(source, pipelineStage);
	if (!shader) {
		POMDOG_THROW_EXCEPTION(std::domain_error, "Failed to compile shader");
	}
}
//-----------------------------------------------------------------------
template <GLenum PipelineStage>
ShaderGL4<PipelineStage>::~ShaderGL4()
{
	if (shader) {
		glDeleteShader(*shader);
		shader = OptionalType::NullOptional;
	}
}
//-----------------------------------------------------------------------
template <GLenum PipelineStage>
GLuint ShaderGL4<PipelineStage>::NativeShader() const
{
	POMDOG_ASSERT(shader);
	return *shader;
}
//-----------------------------------------------------------------------
// explicit instantiations
template class ShaderGL4<GL_VERTEX_SHADER>;
template class ShaderGL4<GL_FRAGMENT_SHADER>;
//template class ShaderGL4<GL_GEOMETRY_SHADER>;
//template class ShaderGL4<GL_TESS_CONTROL_SHADER>;
//template class ShaderGL4<GL_TESS_EVALUATION_SHADER>;
//template class ShaderGL4<GL_COMPUTE_SHADER>;
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
