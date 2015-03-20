//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "EffectPassGL4.hpp"
#include "EffectReflectionGL4.hpp"
#include "ErrorChecker.hpp"
#include "ConstantLayoutGL4.hpp"
#include "InputLayoutGL4.hpp"
#include "ShaderGL4.hpp"
#include "../Utility/ScopeGuard.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Logging/LogLevel.hpp"
#include "Pomdog/Logging/LogStream.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Graphics/EffectPassDescription.hpp"
#include <type_traits>
#include <array>
#include <string>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {
namespace {

static Optional<ShaderProgramGL4> LinkShaders(
	VertexShaderGL4 const& vertexShader, PixelShaderGL4 const& pixelShader)
{
	ShaderProgramGL4 const program {
		glCreateProgram()
	};

	glAttachShader(program.value, vertexShader.NativeShader());
	glAttachShader(program.value, pixelShader.NativeShader());

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
EffectPassGL4::EffectPassGL4(EffectPassDescription const& description)
{
	auto vertexShader = std::dynamic_pointer_cast<VertexShaderGL4>(description.VertexShader);
	if (!vertexShader) {
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Invalid vertex shader.");
	}

	auto pixelShader = std::dynamic_pointer_cast<PixelShaderGL4>(description.PixelShader);
	if (!pixelShader) {
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Invalid pixel shader.");
	}

	POMDOG_ASSERT(vertexShader);
	POMDOG_ASSERT(pixelShader);

	shaderProgram = LinkShaders(*vertexShader, *pixelShader);

	if (!shaderProgram) {
		POMDOG_THROW_EXCEPTION(std::domain_error, "Failed to link shader program.");
	}

	inputLayout = std::make_unique<InputLayoutGL4>(*shaderProgram, description.InputElements);

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
std::unique_ptr<NativeConstantLayout> EffectPassGL4::CreateConstantLayout()
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
InputLayoutGL4* EffectPassGL4::GetInputLayout() const
{
	POMDOG_ASSERT(inputLayout);
	return inputLayout.get();
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog
