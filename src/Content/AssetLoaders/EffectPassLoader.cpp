//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Pomdog/Content/AssetLoaders/EffectPassLoader.hpp"
#include "../../Utility/PathHelper.hpp"
#include "Pomdog/Content/detail/AssetLoaderContext.hpp"
#include "Pomdog/Graphics/ShaderCompilers/GLSLCompiler.hpp"
#include "Pomdog/Graphics/ShaderCompilers/HLSLCompiler.hpp"
#include "Pomdog/Graphics/detail/ShaderBytecode.hpp"
#include "Pomdog/Graphics/EffectPass.hpp"
#include "Pomdog/Graphics/EffectPassDescription.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Graphics/VertexBufferBinding.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <fstream>
#include <vector>
#include <utility>
#include <string>
#include <memory>

namespace Pomdog {
namespace AssetLoaders {
namespace {

static std::vector<std::uint8_t> ReadBinaryFile(std::ifstream && streamIn)
{
	std::ifstream stream = std::move(streamIn);

	if (!stream) {
		return {};
	}

	stream.seekg(0, stream.end);
	auto const length = static_cast<std::size_t>(stream.tellg());
	stream.seekg(0, stream.beg);

	std::vector<std::uint8_t> result(length + 1, 0);
	stream.read(reinterpret_cast<char*>(result.data()), result.size());
	return std::move(result);
}

}// unnamed namespace
//-----------------------------------------------------------------------
class EffectPassLoader::Impl final {
public:
	EffectPassDescription Description;
	Details::AssetLoaderContext LoaderContext;
	std::shared_ptr<GraphicsDevice> GraphicsDevice;
	Optional<std::string> VertexShaderPath;
	Optional<std::string> PixelShaderPath;
	std::string VertexShaderEntryPoint;
	std::string PixelShaderEntryPoint;
};
//-----------------------------------------------------------------------
EffectPassLoader::EffectPassLoader(Details::AssetLoaderContext const& loaderContextIn)
	: impl(std::make_unique<Impl>())
{
	POMDOG_ASSERT(impl);
	impl->LoaderContext = loaderContextIn;
	impl->GraphicsDevice = loaderContextIn.GraphicsDevice.lock();
	POMDOG_ASSERT(impl->GraphicsDevice);
}
//-----------------------------------------------------------------------
EffectPassLoader::EffectPassLoader(EffectPassLoader &&) = default;
EffectPassLoader & EffectPassLoader::operator=(EffectPassLoader &&) = default;
EffectPassLoader::~EffectPassLoader() = default;
//-----------------------------------------------------------------------
EffectPassLoader & EffectPassLoader::VertexShaderGLSL(std::string const& filePath)
{
	POMDOG_ASSERT(!filePath.empty());
	POMDOG_ASSERT(impl);

	if (impl->GraphicsDevice->GetSupportedLanguage() == ShaderLanguage::GLSL) {
		impl->VertexShaderPath = filePath;
	}
	return *this;
}
//-----------------------------------------------------------------------
EffectPassLoader & EffectPassLoader::PixelShaderGLSL(std::string const& filePath)
{
	POMDOG_ASSERT(!filePath.empty());
	POMDOG_ASSERT(impl);

	if (impl->GraphicsDevice->GetSupportedLanguage() == ShaderLanguage::GLSL) {
		impl->PixelShaderPath = filePath;
	}
	return *this;
}
//-----------------------------------------------------------------------
EffectPassLoader & EffectPassLoader::VertexShaderHLSL(std::string const& filePath, std::string const& entryPoint)
{
	POMDOG_ASSERT(!filePath.empty());
	POMDOG_ASSERT(!entryPoint.empty());
	POMDOG_ASSERT(impl);

	if (impl->GraphicsDevice->GetSupportedLanguage() == ShaderLanguage::HLSL) {
		impl->VertexShaderPath = filePath;
		impl->VertexShaderEntryPoint = entryPoint;
	}
	return *this;
}
//-----------------------------------------------------------------------
EffectPassLoader & EffectPassLoader::PixelShaderHLSL(std::string const& filePath, std::string const& entryPoint)
{
	POMDOG_ASSERT(!filePath.empty());
	POMDOG_ASSERT(!entryPoint.empty());
	POMDOG_ASSERT(impl);

	if (impl->GraphicsDevice->GetSupportedLanguage() == ShaderLanguage::HLSL) {
		impl->PixelShaderPath = filePath;
		impl->PixelShaderEntryPoint = entryPoint;
	}
	return *this;
}
//-----------------------------------------------------------------------
EffectPassLoader & EffectPassLoader::InputElements(std::vector<VertexBufferBinding> const& inputElements)
{
	POMDOG_ASSERT(!inputElements.empty());
	POMDOG_ASSERT(impl);
	impl->Description.VertexBindings = inputElements;
	return *this;
}
//-----------------------------------------------------------------------
EffectPassLoader & EffectPassLoader::InputElements(std::vector<VertexBufferBinding> && inputElements)
{
	POMDOG_ASSERT(impl);
	impl->Description.VertexBindings = std::move(inputElements);
	return *this;
}
//-----------------------------------------------------------------------
std::shared_ptr<EffectPass> EffectPassLoader::Load()
{
	POMDOG_ASSERT(impl);
	auto & graphicsDevice = impl->GraphicsDevice;
	auto & effectPassDescription = impl->Description;

	switch (graphicsDevice->GetSupportedLanguage()) {
	case ShaderLanguage::GLSL: {
		if (impl->VertexShaderPath)
		{
			auto shaderSource = ReadBinaryFile(impl->LoaderContext.OpenStream(*impl->VertexShaderPath));

			if (shaderSource.empty()) {
				POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to open file.");
			}

			effectPassDescription.VertexShader = GLSLCompiler::CreateVertexShader(
				*graphicsDevice, shaderSource.data(), shaderSource.size());
		}
		if (impl->PixelShaderPath)
		{
			auto shaderSource = ReadBinaryFile(impl->LoaderContext.OpenStream(*impl->PixelShaderPath));

			if (shaderSource.empty()) {
				POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to open file.");
			}

			effectPassDescription.PixelShader = GLSLCompiler::CreatePixelShader(
				*graphicsDevice, shaderSource.data(), shaderSource.size());
		}
		break;
	}
	case ShaderLanguage::HLSL: {
		if (impl->VertexShaderPath)
		{
			auto shaderSource = ReadBinaryFile(impl->LoaderContext.OpenStream(*impl->VertexShaderPath));

			if (shaderSource.empty()) {
				POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to open file.");
			}

			effectPassDescription.VertexShader = HLSLCompiler::CreateVertexShader(
				*graphicsDevice, shaderSource.data(), shaderSource.size(), impl->VertexShaderEntryPoint);
		}
		if (impl->PixelShaderPath)
		{
			auto shaderSource = ReadBinaryFile(impl->LoaderContext.OpenStream(*impl->PixelShaderPath));

			if (shaderSource.empty()) {
				POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to open file.");
			}

			effectPassDescription.PixelShader = HLSLCompiler::CreatePixelShader(
				*graphicsDevice, shaderSource.data(), shaderSource.size(), impl->PixelShaderEntryPoint);
		}
		break;
	}
	case ShaderLanguage::Metal: {
//		auto library = MetalCompiler::CompileLibrary(graphicsDevice, "float4 DeferredVS{} float4 DeferredPS{}");
//		effectPassDescription.VertexShader = MetalCompiler::CreateVertexShader(library, "DeferredVS");
//		effectPassDescription.PixelShader = MetalCompiler::CreatePixelShader(library, "DeferredPS");
		break;
	}
	}

	auto effectPass = std::make_shared<EffectPass>(graphicsDevice, effectPassDescription);
	return std::move(effectPass);
}
//-----------------------------------------------------------------------
}// namespace AssetLoaders
}// namespace Pomdog
