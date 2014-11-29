//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "EffectPassGL4.hpp"
#include "EffectReflectionGL4.hpp"
#include "ErrorChecker.hpp"
#include "ConstantLayoutGL4.hpp"
#include "../Utility/ScopeGuard.hpp"
#include <Pomdog/Logging/Log.hpp>
#include <Pomdog/Logging/LogLevel.hpp>
#include <Pomdog/Logging/LogStream.hpp>
#include <Pomdog/Utility/Exception.hpp>
#include <Pomdog/Graphics/detail/ShaderBytecode.hpp>
#include <type_traits>
#include <array>
#include <string>

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

	auto result = MakeTagged<GLuint, Tag>(glCreateShader(pipelineStage));

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

		Log::Stream(LogLevel::Critical)
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

	#ifdef DEBUG
	ErrorChecker::CheckError("glLinkProgram", __FILE__, __LINE__);
	#endif

	GLint linkSuccess = 0;
	glGetProgramiv(program.value, GL_LINK_STATUS, &linkSuccess);

	if (linkSuccess == GL_FALSE)
	{
#ifdef DEBUG
		std::array<GLchar, 256> messageBuffer;

		glGetProgramInfoLog(program.value, messageBuffer.size(), 0, messageBuffer.data());
		std::string const message = messageBuffer.data();

		Log::Stream(LogLevel::Critical)
			<< "Failed to link shaders.\nerror: " << message.c_str();
#endif // defined(DEBUG)

		glDeleteProgram(program.value);
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
			"Failed to compile vertex shader.");
	}
	
	ScopeGuard vertexScope([&]{
		POMDOG_ASSERT(vertexShader);
		glDeleteShader(vertexShader->value);
	});

	auto pixelShader = CompileShader<PixelShaderGL4::tag_type>(pixelShaderBytecode);
	if (!pixelShader) {
		POMDOG_THROW_EXCEPTION(std::domain_error,
			"Failed to compile pixel shader.");
	}
	
	ScopeGuard pixelScope([&]{
		POMDOG_ASSERT(pixelShader);
		glDeleteShader(pixelShader->value);
	});
	
	POMDOG_ASSERT(vertexShader);
	POMDOG_ASSERT(pixelShader);
	
	shaderProgram = LinkShaders(*vertexShader, *pixelShader);
	
	if (!pixelShader) {
		POMDOG_THROW_EXCEPTION(std::domain_error,
			"Failed to link shader program.");
	}

	EffectReflectionGL4 shaderReflection(*shaderProgram);
	{
		auto uniformBlocks = shaderReflection.GetNativeUniformBlocks();

		std::uint16_t slotIndex = 0;
		for (auto & uniformBlock: uniformBlocks)
		{
			glUniformBlockBinding(shaderProgram->value, uniformBlock.BlockIndex, slotIndex);
			uniformBlockBindings.push_back({uniformBlock.Name, slotIndex});
			++slotIndex;
		}
	}
	{
		auto uniforms = shaderReflection.GetNativeUniforms();

		std::uint16_t slotIndex = 0;
		for (auto & uniform: uniforms)
		{
			switch (uniform.Type) {
			case GL_SAMPLER_1D:
			case GL_SAMPLER_2D:
			case GL_SAMPLER_3D:
			case GL_SAMPLER_CUBE:
			case GL_SAMPLER_1D_SHADOW:
			case GL_SAMPLER_2D_SHADOW:
			case GL_SAMPLER_1D_ARRAY:
			case GL_SAMPLER_2D_ARRAY:
			case GL_SAMPLER_1D_ARRAY_SHADOW:
			case GL_SAMPLER_2D_ARRAY_SHADOW:
			case GL_SAMPLER_2D_MULTISAMPLE:
			case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
			case GL_SAMPLER_CUBE_SHADOW:
			case GL_SAMPLER_BUFFER:
			case GL_SAMPLER_2D_RECT:
			case GL_SAMPLER_2D_RECT_SHADOW:
				break;
			default:
				continue;
				break;
			}
			
			TextureBindingGL4 binding;
			binding.UniformLocation = uniform.Location;
			binding.SlotIndex = slotIndex;
			textureBindings.push_back(binding);
			++slotIndex;
		}
	}
}
//-----------------------------------------------------------------------
EffectPassGL4::~EffectPassGL4()
{
	if (shaderProgram) {
		glDeleteProgram(shaderProgram->value);
	}
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeConstantLayout> EffectPassGL4::CreateConstantLayout(NativeGraphicsDevice &)
{
	EffectReflectionGL4 shaderReflection(*shaderProgram);

	std::vector<ConstantBufferBindingGL4> bindings;
	
	for (auto & uniformBlock: uniformBlockBindings)
	{
		ConstantBufferBindingGL4 binding;
		binding.Name = uniformBlock.Name;
		binding.SlotIndex = uniformBlock.SlotIndex;
		bindings.push_back(std::move(binding));
	}
	
	return std::make_unique<ConstantLayoutGL4>(std::move(bindings));
}
//-----------------------------------------------------------------------
void EffectPassGL4::ApplyShaders()
{
	POMDOG_ASSERT(shaderProgram);
	glUseProgram(shaderProgram->value);

	#ifdef DEBUG
	ErrorChecker::CheckError("glUseProgram", __FILE__, __LINE__);
	#endif

	for (auto & binding: textureBindings)
	{
		glUniform1i(binding.UniformLocation, binding.SlotIndex);
		
		#ifdef DEBUG
		ErrorChecker::CheckError("glUniform1i", __FILE__, __LINE__);
		#endif
	}
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
