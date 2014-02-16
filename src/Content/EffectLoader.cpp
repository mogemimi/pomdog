//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Content/detail/EffectLoader.hpp>
#include <fstream>
#include <vector>
#include <utility>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include <Pomdog/Graphics/EffectPass.hpp>
#include <Pomdog/Graphics/detail/ShaderBytecode.hpp>
#include <Pomdog/Utility/Exception.hpp>

namespace Pomdog {
namespace Details {
namespace {

std::vector<std::uint8_t> ReadBinaryFile(std::string const& filename)
{
	std::ifstream is(filename, std::ios::binary);
	
	is.seekg (0, is.end);
	auto const length = is.tellg();
	is.seekg (0, is.beg);
	
	std::vector<std::uint8_t> result(length, 0);
	is.read(reinterpret_cast<char*>(result.data()), result.size());
	return std::move(result);
}

}// unnamed namespace

template <>
std::shared_ptr<EffectPass> AssetLoader<EffectPass>::operator()(AssetLoaderContext const& loaderContext,
	std::string const& assetPath)
{
	auto const vertexShader = ReadBinaryFile(loaderContext.rootDirectory + "/" + assetPath + "/VertexShader.glsl");
	auto const pixelShader = ReadBinaryFile(loaderContext.rootDirectory + "/" + assetPath + "/PixelShader.glsl");

	auto graphicsContext = loaderContext.graphicsContext.lock();
	auto graphicsDevice = loaderContext.graphicsDevice.lock();
	
	if (!graphicsContext || !graphicsDevice) {
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Invalid graphics context or device.");
	}

	auto effectPass = std::make_shared<EffectPass>(graphicsDevice, graphicsContext,
		Details::ShaderBytecode { vertexShader.data(), vertexShader.size() },
		Details::ShaderBytecode { pixelShader.data(), pixelShader.size() });

	return std::move(effectPass);
}

}// namespace Details
}// namespace Pomdog
