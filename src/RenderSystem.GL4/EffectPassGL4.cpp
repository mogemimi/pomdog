//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "EffectPassGL4.hpp"
#include <type_traits>
#include <array>
#include <string>
#include <algorithm>
#include <Pomdog/Logging/Log.hpp>
#include <Pomdog/Logging/LoggingLevel.hpp>
#include <Pomdog/Logging/LogStream.hpp>
#include <Pomdog/Utility/Exception.hpp>
#include <Pomdog/Graphics/GraphicsContext.hpp>
#include <Pomdog/Graphics/detail/ShaderBytecode.hpp>
#include "../Utility/ScopeGuard.hpp"
#include "ConstantBufferGL4.hpp"
#include "GraphicsContextGL4.hpp"
#include "ErrorChecker.hpp"
#include "EffectReflectionGL4.hpp"

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

		Log::Stream(LoggingLevel::Critical)
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
	
	// Create default constant buffers:
	EffectReflectionGL4 shaderReflection(*shaderProgram);
	auto uniformBlocks = shaderReflection.GetNativeUniformBlocks();
	{
		std::uint32_t slotIndex = 0;
		for (auto & uniformBlock: uniformBlocks) {
			ConstantBufferBindingGL4 binding;
			//binding.ConstantBuffer = std::make_shared<ConstantBufferGL4>(uniformBlock.ByteConstants);
			binding.Name = uniformBlock.Name;
			binding.SlotIndex = slotIndex;
			
			glUniformBlockBinding(shaderProgram->value, uniformBlock.BlockIndex, binding.SlotIndex);
			constantBufferBindings.push_back(std::move(binding));
			++slotIndex;
		}
	}
}
//-----------------------------------------------------------------------
EffectPassGL4::~EffectPassGL4()
{
	constantBufferBindings.clear();

	if (shaderProgram) {
		glDeleteProgram(shaderProgram->value);
	}
}
//-----------------------------------------------------------------------
void EffectPassGL4::SetConstant(std::string const& constantName, std::shared_ptr<NativeConstantBuffer> const& constantBuffer)
{
	auto iter = std::find_if(std::begin(constantBufferBindings), std::end(constantBufferBindings),
		[&constantName](ConstantBufferBindingGL4 const& binding) { return binding.Name == constantName; });

	POMDOG_ASSERT(std::end(constantBufferBindings) != iter);

	auto nativeConstantBuffer = std::dynamic_pointer_cast<ConstantBufferGL4>(constantBuffer);
	POMDOG_ASSERT(nativeConstantBuffer);

	if (std::end(constantBufferBindings) != iter) {
		iter->ConstantBuffer = nativeConstantBuffer;
	}
}
//-----------------------------------------------------------------------
void EffectPassGL4::SetConstant(std::string const& constantName)
{
	auto iter = std::find_if(std::begin(constantBufferBindings), std::end(constantBufferBindings),
		[&constantName](ConstantBufferBindingGL4 const& binding) { return binding.Name == constantName; });

	POMDOG_ASSERT(std::end(constantBufferBindings) != iter);

	if (std::end(constantBufferBindings) != iter) {
		iter->ConstantBuffer.reset();
	}
}
//-----------------------------------------------------------------------
void EffectPassGL4::Apply(GraphicsContext & graphicsContext,
	std::shared_ptr<EffectPass> const& sharedThisEffectPass)
{
	POMDOG_ASSERT(shaderProgram);
	POMDOG_ASSERT(sharedThisEffectPass);
	
	auto sharedThis = std::shared_ptr<EffectPassGL4>(sharedThisEffectPass, this);
	auto nativeContext = dynamic_cast<GraphicsContextGL4*>(graphicsContext.GetNativeGraphicsContext());
	POMDOG_ASSERT(nativeContext);
	
	if (nativeContext != nullptr) {
		nativeContext->SetEffectPass(sharedThis);
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
	
	// Apply constant buffers:
	for (auto & binding: constantBufferBindings)
	{
		if (binding.ConstantBuffer) {
			binding.ConstantBuffer->Apply(binding.SlotIndex);
		}
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
