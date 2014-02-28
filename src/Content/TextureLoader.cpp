//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Content/detail/TextureLoader.hpp>
#include <fstream>
#include <array>
#include <vector>
#include <algorithm>
#include <utility>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Utility/Exception.hpp>
#include "Utility/MakeFourCC.hpp"
#include "Utility/DDSTextureReader.hpp"
#include "Utility/PNGTextureReader.hpp"

namespace Pomdog {
namespace Details {
namespace {

static std::string MakeAssetFilePath(AssetLoaderContext const& loaderContext, std::string const& assetPath)
{
	POMDOG_ASSERT(!assetPath.empty());
	return loaderContext.RootDirectory + "/" + assetPath;
}
//-----------------------------------------------------------------------
namespace BinaryReader
{
	template <class Stream>
	static std::size_t GetBinarySize(Stream & stream)
	{
		stream.seekg(0, stream.end);
		auto const length = stream.tellg();
		stream.seekg(0, stream.beg);
		return length;
	}

	template <typename T, class Stream>
	std::vector<T> ReadArray(Stream & stream, std::size_t elementCount)
	{
		static_assert(std::is_pod<T>::value, "");
		
		std::vector<T> result(elementCount);
		stream.read(reinterpret_cast<char*>(result.data()), sizeof(T) * result.size());
		return std::move(result);
	}
	
	template <typename T, std::size_t ElementCount, class Stream>
	std::array<T, ElementCount> ReadArray(Stream & stream)
	{
		static_assert(std::is_pod<T>::value, "");
		
		std::array<T, ElementCount> result;
		stream.read(reinterpret_cast<char*>(result.data()), sizeof(T) * result.size());
		return std::move(result);
	}
	
	template <typename T, class Stream>
	T Read(Stream & stream)
	{
		static_assert(std::is_pod<T>::value, "");
		return *ReadArray<T>(stream, 1);
	}
}
//-----------------------------------------------------------------------
static bool IsPNGFormat(std::array<std::uint8_t, 8> const& signature)
{
	std::array<std::uint8_t, 8> const pngSignature {{
		0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}};

	return std::equal(std::begin(signature), std::end(signature),
		std::begin(pngSignature));
}
//-----------------------------------------------------------------------
static bool IsDDSFormat(std::array<std::uint8_t, 8> const& signature)
{
	constexpr auto fourCC = MakeFourCC('D','D','S',' ');
	static_assert(fourCC == 0x20534444, "The four character code value is 'DDS '");
	return (MakeFourCC(signature[0], signature[1], signature[2], signature[3]) == fourCC);
}

}// unnamed namespace
//-----------------------------------------------------------------------
template <>
std::shared_ptr<Texture2D> AssetLoader<Texture2D>::operator()(AssetLoaderContext const& loaderContext,
	std::string const& assetPath)
{
	auto filePath = MakeAssetFilePath(loaderContext, assetPath);
	
	std::ifstream stream(filePath, std::ios::binary);
	if (stream.fail()) {
		POMDOG_THROW_EXCEPTION(std::invalid_argument, "Cannot open file: " + filePath);
	}
	
	auto const fileSize = BinaryReader::GetBinarySize(stream);
	if (fileSize < (sizeof(std::uint8_t) * 8)) {
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented.");
	}

	auto signature = BinaryReader::ReadArray<std::uint8_t, 8>(stream);
	if (stream.fail()) {
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented.");
	}
	
	if (IsPNGFormat(signature))
	{
		constexpr auto SignatureLength = sizeof(std::uint8_t) * 8;
	
		POMDOG_ASSERT(stream.tellg() == SignatureLength);
		
		std::vector<std::uint8_t> binaryWithoutSignature(fileSize - SignatureLength);
		stream.read(reinterpret_cast<char*>(binaryWithoutSignature.data()), binaryWithoutSignature.size());
		
		auto graphicsDevice = loaderContext.GraphicsDevice.lock();
		POMDOG_ASSERT(graphicsDevice);
		
		return PNGTextureReader::Read(graphicsDevice, binaryWithoutSignature.data(), binaryWithoutSignature.size());
	}
	else if (IsDDSFormat(signature))
	{
		constexpr auto FourCCLength = sizeof(std::uint8_t) * 4;
		
		stream.seekg(FourCCLength, stream.beg);
		
		std::vector<std::uint8_t> binaryWithoutFourCC(fileSize - FourCCLength);
		stream.read(reinterpret_cast<char*>(binaryWithoutFourCC.data()), binaryWithoutFourCC.size());
		
		auto graphicsDevice = loaderContext.GraphicsDevice.lock();
		POMDOG_ASSERT(graphicsDevice);
		
		return DDSTextureReader::Read(graphicsDevice, binaryWithoutFourCC.data(), binaryWithoutFourCC.size());
	}
	else
	{
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Invalid/unsupported texture format.");
	}

	POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented.");
}
//-----------------------------------------------------------------------
}// namespace Details
}// namespace Pomdog
