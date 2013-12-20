//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "EffectPassGL4.hpp"
#include <type_traits>
#include <array>
#include <string>
#include <Pomdog/Logging/Log.hpp>
#include <Pomdog/Logging/LoggingLevel.hpp>
#include <Pomdog/Logging/LogStream.hpp>
#include <Pomdog/Utility/Exception.hpp>
#include "../Utility/ScopeGuard.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "ErrorChecker.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {
//-----------------------------------------------------------------------
namespace {
namespace Tags {

struct VertexShaderTag {};
struct PixelShaderTag {};

}// namespace Tags
//-----------------------------------------------------------------------
using VertexShaderGL4 = Tagged<GLuint, Tags::VertexShaderTag>;
using PixelShaderGL4 = Tagged<GLuint, Tags::PixelShaderTag>;
//-----------------------------------------------------------------------
template <class Tag> static
Optional<Tagged<GLuint, Tag>>
CompileShader(ShaderBytecode const& source)
{
	static_assert(std::is_same<Tag, Tags::VertexShaderTag>::value ||
		std::is_same<Tag, Tags::PixelShaderTag>::value, "You can only use 'vertex' or 'pixel' shader.");

	constexpr auto pipelineStage = typename std::conditional<
		std::is_same<Tag, Tags::VertexShaderTag>::value,
		std::integral_constant<GLenum, GL_VERTEX_SHADER>,
		std::integral_constant<GLenum, GL_FRAGMENT_SHADER>
	>::type();
	
	static_assert((std::is_same<Tag, Tags::VertexShaderTag>::value && (pipelineStage == GL_VERTEX_SHADER)) ||
		(std::is_same<Tag, Tags::PixelShaderTag>::value && (pipelineStage == GL_FRAGMENT_SHADER)), "");

	Tagged<GLuint, Tag> result{
		glCreateShader(pipelineStage)
	};

	std::array<GLchar const*, 1> shaderSource = {{
		reinterpret_cast<GLchar const*>(source.Code)
	}};

	POMDOG_ASSERT(source.ByteLength < std::numeric_limits<GLint>::max());
	GLint const sourceLength = static_cast<GLint>(source.ByteLength);

	glShaderSource(result.value, 1, shaderSource.data(), &sourceLength);

	glCompileShader(result.value);

	GLint compileSuccess = 0;
	glGetShaderiv(result.value, GL_COMPILE_STATUS, &compileSuccess);

	if (compileSuccess == GL_FALSE)
	{
#ifdef DEBUG
		std::array<GLchar, 256> messageBuffer;

		glGetShaderInfoLog(result.value, messageBuffer.size(), 0, messageBuffer.data());
		std::string const message = messageBuffer.data();

		Log::Stream(LoggingLevel::Critical)
			<< "Failed to compile shader.\nerror: " << message;
#endif // defined(DEBUG)

		glDeleteShader(result.value);
		return OptionalType::NullOptional;
	}

	return result;
}
//-----------------------------------------------------------------------
static
Optional<ShaderProgramGL4>
LinkShaders(VertexShaderGL4 const& vertexShader, PixelShaderGL4 const& pixelShader)
{
	ShaderProgramGL4 const program {
		glCreateProgram()
	};
		
	glAttachShader(program.value, vertexShader.value);
	glAttachShader(program.value, pixelShader.value);

	glLinkProgram(program.value);

	GLint linkSuccess = 0;
	glGetProgramiv(program.value, GL_LINK_STATUS, &linkSuccess);

	if (linkSuccess == GL_FALSE)
	{
#ifdef DEBUG
		std::array<GLchar, 256> messageBuffer;

		glGetProgramInfoLog(program.value, messageBuffer.size(), 0, messageBuffer.data());
		std::string const message = messageBuffer.data();

		Log::Stream(LoggingLevel::Critical)
			<< "Failed to link shaders.\nerror: " << message.c_str();
#endif // defined(DEBUG)

		glDeleteShader(program.value);
		return OptionalType::NullOptional;
	}
	return program;
}

}// unnamed namespace
//-----------------------------------------------------------------------
EffectPassGL4::EffectPassGL4(ShaderBytecode const& vertexShaderBytecode,
	ShaderBytecode const& pixelShaderBytecode)
{
	auto vertexShader = CompileShader<VertexShaderGL4::tag_type>(vertexShaderBytecode);
	if (!vertexShader) {
		POMDOG_THROW_EXCEPTION(std::domain_error,
			"Failed to compile vertex shader.", "EffectPassGL4::EffectPassGL4");
	}
	
	ScopeGuard vertexScope([&](){
		POMDOG_ASSERT(vertexShader);
		glDeleteShader(vertexShader->value);
	});

	auto pixelShader = CompileShader<PixelShaderGL4::tag_type>(pixelShaderBytecode);
	if (!pixelShader) {
		POMDOG_THROW_EXCEPTION(std::domain_error,
			"Failed to compile pixel shader.", "EffectPassGL4::EffectPassGL4");
	}
	
	ScopeGuard pixelScope([&](){
		POMDOG_ASSERT(pixelShader);
		glDeleteShader(pixelShader->value);
	});
	
	POMDOG_ASSERT(vertexShader);
	POMDOG_ASSERT(pixelShader);
	
	shaderProgram = LinkShaders(*vertexShader, *pixelShader);
	
	if (!pixelShader) {
		POMDOG_THROW_EXCEPTION(std::domain_error,
			"Failed to link shader program.", "EffectPassGL4::EffectPassGL4");
	}
	
	// build attributes
	// build uniforms
	// build input layout
	// build effect parameters
}
//-----------------------------------------------------------------------
EffectPassGL4::~EffectPassGL4()
{
	if (shaderProgram) {
		glDeleteProgram(shaderProgram->value);
	}
}
//-----------------------------------------------------------------------
void EffectPassGL4::ApplyShaders()
{
	POMDOG_ASSERT(shaderProgram);
	glUseProgram(shaderProgram->value);

	#ifdef DEBUG
	ErrorChecker::CheckError("glUseProgram", __FILE__, __LINE__);
	#endif

	//POMDOG_ASSERT(uniforms);
	//uniforms->ApplyUniforms();
}
//-----------------------------------------------------------------------
ShaderProgramGL4 EffectPassGL4::GetShaderProgram() const
{
	POMDOG_ASSERT(shaderProgram);
	return shaderProgram.value();
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
