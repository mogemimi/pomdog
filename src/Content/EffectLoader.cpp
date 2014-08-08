//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Content/detail/EffectLoader.hpp>
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/EffectPass.hpp"
#include "Pomdog/Graphics/detail/ShaderBytecode.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <fstream>
#include <vector>
#include <utility>

namespace Pomdog {
namespace Details {
namespace {

static std::vector<std::uint8_t> ReadBinaryFile(std::string const& filename)
{
	std::ifstream stream(filename, std::ios::binary);
	
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
std::shared_ptr<EffectPass> AssetLoader<EffectPass>::operator()(AssetLoaderContext const& loaderContext,
	std::string const& assetPath)
{
	auto const vertexShader = ReadBinaryFile(loaderContext.RootDirectory + "/" + assetPath + "/VertexShader.glsl");
	auto const pixelShader = ReadBinaryFile(loaderContext.RootDirectory + "/" + assetPath + "/PixelShader.glsl");

	POMDOG_ASSERT(!vertexShader.empty());
	POMDOG_ASSERT(!pixelShader.empty());

	if (vertexShader.empty() || pixelShader.empty()) {
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to open file.");
	}

	auto graphicsDevice = loaderContext.GraphicsDevice.lock();
	
	if (!graphicsDevice) {
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Invalid graphics device.");
	}

	auto effectPass = std::make_shared<EffectPass>(graphicsDevice,
		Details::ShaderBytecode { vertexShader.data(), vertexShader.size() },
		Details::ShaderBytecode { pixelShader.data(), pixelShader.size() });

	return std::move(effectPass);
}

}// namespace Details
}// namespace Pomdog
