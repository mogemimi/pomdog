//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Pomdog/Graphics/ShaderCompilers/HLSLCompiler.hpp"
#include "../../RenderSystem/NativeGraphicsDevice.hpp"
#include "../../RenderSystem/ShaderBytecode.hpp"
#include "../../RenderSystem/ShaderCompileOptions.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace ShaderCompilers {

using Detail::RenderSystem::ShaderBytecode;
using Detail::RenderSystem::ShaderCompileOptions;
using Detail::RenderSystem::ShaderPipelineStage;

static std::unique_ptr<Shader> CreateShaderFromPrecompiledBinary(GraphicsDevice & graphicsDevice,
	void const* shaderSource, std::size_t byteLength, ShaderPipelineStage pipelineStage)
{
	POMDOG_ASSERT(shaderSource != nullptr);
	POMDOG_ASSERT(byteLength > 0);
	POMDOG_ASSERT(graphicsDevice.GetSupportedLanguage() == ShaderLanguage::HLSL);

	auto nativeGraphicsDevice = graphicsDevice.NativeGraphicsDevice();

	ShaderBytecode shaderBytecode;
	shaderBytecode.Code = shaderSource;
	shaderBytecode.ByteLength = byteLength;

	ShaderCompileOptions compileOptions;
	compileOptions.Profile.PipelineStage = pipelineStage;
	compileOptions.Precompiled = true;

	return nativeGraphicsDevice->CreateShader(shaderBytecode, compileOptions);
}
//-----------------------------------------------------------------------
static std::unique_ptr<Shader> CompileHLSLShaderFromSource(GraphicsDevice & graphicsDevice,
	void const* shaderSource, std::size_t byteLength,
	std::string const& entryPoint, ShaderPipelineStage pipelineStage)
{
	POMDOG_ASSERT(shaderSource != nullptr);
	POMDOG_ASSERT(byteLength > 0);
	POMDOG_ASSERT(graphicsDevice.GetSupportedLanguage() == ShaderLanguage::HLSL);

	auto nativeGraphicsDevice = graphicsDevice.NativeGraphicsDevice();

	ShaderBytecode shaderBytecode;
	shaderBytecode.Code = shaderSource;
	shaderBytecode.ByteLength = byteLength;

	ShaderCompileOptions compileOptions;
	compileOptions.EntryPoint = entryPoint;
	compileOptions.Profile.PipelineStage = pipelineStage;
	compileOptions.Profile.ShaderModel.Major = 4;
	compileOptions.Profile.ShaderModel.Minor = 0;
	compileOptions.Precompiled = false;

	return nativeGraphicsDevice->CreateShader(shaderBytecode, compileOptions);
}
//-----------------------------------------------------------------------
std::unique_ptr<Shader> HLSLCompiler::CreateVertexShader(
	GraphicsDevice & graphicsDevice, void const* shaderSource, std::size_t byteLength)
{
	return CreateShaderFromPrecompiledBinary(graphicsDevice, shaderSource, byteLength,
		ShaderPipelineStage::VertexShader);
}
//-----------------------------------------------------------------------
std::unique_ptr<Shader> HLSLCompiler::CreatePixelShader(
	GraphicsDevice & graphicsDevice, void const* shaderSource, std::size_t byteLength)
{
	return CreateShaderFromPrecompiledBinary(graphicsDevice, shaderSource, byteLength,
		ShaderPipelineStage::PixelShader);
}
//-----------------------------------------------------------------------
std::unique_ptr<Shader> HLSLCompiler::CreateVertexShaderFromSource(
	GraphicsDevice & graphicsDevice, void const* shaderSource, std::size_t byteLength,
	std::string const& entryPoint)
{
	return CompileHLSLShaderFromSource(graphicsDevice, shaderSource, byteLength,
		entryPoint, ShaderPipelineStage::VertexShader);
}
//-----------------------------------------------------------------------
std::unique_ptr<Shader> HLSLCompiler::CreatePixelShaderFromSource(
	GraphicsDevice & graphicsDevice, void const* shaderSource, std::size_t byteLength,
	std::string const& entryPoint)
{
	return CompileHLSLShaderFromSource(graphicsDevice, shaderSource, byteLength,
		entryPoint, ShaderPipelineStage::PixelShader);
}
//-----------------------------------------------------------------------
}// namespace ShaderCompilers
}// namespace Pomdog
