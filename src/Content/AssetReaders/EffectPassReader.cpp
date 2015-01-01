//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "EffectPassReader.hpp"
#include "../../Utility/PathHelper.hpp"
#include "Pomdog/Content/detail/AssetLoaderContext.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/EffectPass.hpp"
#include "Pomdog/Graphics/detail/ShaderBytecode.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <fstream>
#include <vector>
#include <utility>
#include <string>
#include <memory>

namespace Pomdog {
namespace Details {
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
std::shared_ptr<EffectPass> EffectPassReader::Read(
	AssetLoaderContext const& loaderContext, std::string const& assetName)
{
	auto const vertexShader = ReadBinaryFile(loaderContext.OpenStream(
		PathHelper::Join(assetName, "/VertexShader.glsl")));
	auto const pixelShader = ReadBinaryFile(loaderContext.OpenStream(
		PathHelper::Join(assetName, "/PixelShader.glsl")));

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
//-----------------------------------------------------------------------
}// namespace Details
}// namespace Pomdog
