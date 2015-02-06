//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Pomdog/Graphics/ShaderCompilers/GLSLCompiler.hpp"
#include "../../RenderSystem/NativeGraphicsDevice.hpp"
#include "../../RenderSystem/ShaderCompileOptions.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Graphics/detail/ShaderBytecode.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
std::unique_ptr<Shader> GLSLCompiler::CreateVertexShader(
	GraphicsDevice & graphicsDevice, void const* source, std::size_t byteLength)
{
	POMDOG_ASSERT(graphicsDevice.GetSupportedLanguage() == ShaderLanguage::GLSL);

	using Details::ShaderBytecode;
	using Details::RenderSystem::ShaderCompileOptions;
	using Details::RenderSystem::ShaderPipelineStage;

	auto nativeGraphicsDevice = graphicsDevice.NativeGraphicsDevice();

	ShaderBytecode shaderBytecode;
	shaderBytecode.Code = source;
	shaderBytecode.ByteLength = byteLength;

	ShaderCompileOptions compileOptions;
	compileOptions.EntryPoint = "main";
	compileOptions.Profile.PipelineStage = ShaderPipelineStage::VertexShader;

	return nativeGraphicsDevice->CreateShader(shaderBytecode, compileOptions);
}
//-----------------------------------------------------------------------
std::unique_ptr<Shader> GLSLCompiler::CreatePixelShader(
	GraphicsDevice & graphicsDevice, void const* source, std::size_t byteLength)
{
	POMDOG_ASSERT(graphicsDevice.GetSupportedLanguage() == ShaderLanguage::GLSL);

	using Details::ShaderBytecode;
	using Details::RenderSystem::ShaderCompileOptions;
	using Details::RenderSystem::ShaderPipelineStage;

	auto nativeGraphicsDevice = graphicsDevice.NativeGraphicsDevice();

	ShaderBytecode shaderBytecode;
	shaderBytecode.Code = source;
	shaderBytecode.ByteLength = byteLength;

	ShaderCompileOptions compileOptions;
	compileOptions.EntryPoint = "main";
	compileOptions.Profile.PipelineStage = ShaderPipelineStage::PixelShader;

	return nativeGraphicsDevice->CreateShader(shaderBytecode, compileOptions);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
