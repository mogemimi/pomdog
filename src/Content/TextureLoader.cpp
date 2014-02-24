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
extern "C" {
	#include <png.h>
}
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Utility/Exception.hpp>
#include <Pomdog/Graphics/Texture2D.hpp>
#include <Pomdog/Graphics/SurfaceFormat.hpp>
#include "../Utility/ScopeGuard.hpp"

namespace Pomdog {
namespace Details {
namespace {

static std::string MakeAssetFilePath(AssetLoaderContext const& loaderContext, std::string const& assetPath)
{
	POMDOG_ASSERT(!assetPath.empty());
	return loaderContext.RootDirectory + "/" + assetPath;
}

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

static bool IsPNGFormat(std::array<std::uint8_t, 8> const& signature)
{
	std::array<std::uint8_t, 8> const pngSignature {{
		0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}};

	return std::equal(std::begin(signature), std::end(signature),
		std::begin(pngSignature));
}

struct ImageBinary
{
	std::uint8_t const* Data;
	std::size_t ByteLength;
};

struct PNGBinaryContext
{
	std::uint8_t const* Data;
	std::size_t ByteLength;
	std::size_t Offset;
};

struct Texture2DParsingData
{
	std::vector<std::uint8_t> Binary;
	std::uint32_t Height;
	std::uint32_t Width;
	SurfaceFormat Format;
};

static void ReadPNGDataCallback(::png_structp png_ptr, ::png_bytep data, ::png_size_t length)
{
	auto context = static_cast<PNGBinaryContext*>(::png_get_io_ptr(png_ptr));

	if ((context->Offset + length) <= context->ByteLength) {
		std::memcpy(data, context->Data + context->Offset, length);
		context->Offset += length;
	}
	else {
		png_error(png_ptr, "ReadPngCallback failed.");
	}
}

static Texture2DParsingData ReadPNG(ImageBinary const& binary)
{
	auto pngPtr = ::png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (nullptr == pngPtr) {
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented.");
	}
	
	ScopeGuard scopedDestroyReadStruct([&] {
		if (nullptr != pngPtr) {
			::png_destroy_read_struct(&pngPtr, nullptr, nullptr);
		}
	});
	
	auto infoPtr = ::png_create_info_struct(pngPtr);
	if (nullptr == infoPtr) {
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented.");
	}
	
	PNGBinaryContext context;
	context.Data = binary.Data;
	context.ByteLength = binary.ByteLength;
	context.Offset = 0;
	::png_set_read_fn(pngPtr, &context, ReadPNGDataCallback);
	
	constexpr auto pngSignatureByteLength = sizeof(std::uint8_t) * 8;
	::png_set_sig_bytes(pngPtr, pngSignatureByteLength);
	
	// Read PNG Header
	::png_read_info(pngPtr, infoPtr);
	auto const originalColorType = ::png_get_color_type(pngPtr, infoPtr);
	auto const originalBitDepth = ::png_get_bit_depth(pngPtr, infoPtr);
	
	// Settings
	if ((PNG_COLOR_TYPE_GRAY == originalColorType) && (originalBitDepth < 8)) {
		::png_set_expand_gray_1_2_4_to_8(pngPtr);
	}
	else if (originalBitDepth < 8) {
		::png_set_packing(pngPtr);
	}
	else if (originalBitDepth == 16) {
		::png_set_strip_16(pngPtr);
	}
	
	if (PNG_COLOR_TYPE_PALETTE == originalColorType) {
		::png_set_palette_to_rgb(pngPtr);
	}
	
	if (::png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS)) {
		::png_set_tRNS_to_alpha(pngPtr);
	}
	
	::png_read_update_info(pngPtr, infoPtr);
	
	// Texture2D Info
	auto const pixelWidth = ::png_get_image_width(pngPtr, infoPtr);
	auto const pixelHeight = ::png_get_image_height(pngPtr, infoPtr);
	auto const colorType = ::png_get_color_type(pngPtr, infoPtr);
	//auto const bitDepth = ::png_get_bit_depth(pngPtr, infoPtr);
	
	POMDOG_ASSERT(pixelWidth > 0);
	POMDOG_ASSERT(pixelHeight > 0);
	
	// Read PNG Image Data
	auto const rowBytes = ::png_get_rowbytes(pngPtr, infoPtr);
	std::vector<std::uint8_t> rowData(rowBytes * pixelHeight);
	
	std::vector<::png_bytep> bytePointers(pixelHeight, nullptr);
	
	for (std::uint32_t index = 0; index < pixelHeight; ++index) {
		bytePointers[index] = &rowData[index];
	}
	
	::png_read_image(pngPtr, bytePointers.data());
	::png_read_end(pngPtr, nullptr);
	
	Texture2DParsingData parsingData;
	parsingData.Width = pixelWidth;
	parsingData.Height = pixelHeight;
	parsingData.Format = ([](::png_byte colorType)->SurfaceFormat{
		switch (colorType) {
		case PNG_COLOR_TYPE_GRAY:
			return SurfaceFormat::R8_UNorm;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			return SurfaceFormat::R8G8_UNorm;
		case PNG_COLOR_TYPE_RGB:
			return SurfaceFormat::R8G8B8_UNorm;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			return SurfaceFormat::R8G8B8A8_UNorm;
		default:
			POMDOG_ASSERT(colorType != PNG_COLOR_TYPE_PALETTE);
			break;
		}
		return SurfaceFormat::A8_UNorm;
	})(colorType);
	
	parsingData.Binary = std::move(rowData);
	
	return std::move(parsingData);
}

}// unnamed namespace

template <>
std::shared_ptr<Texture2D> AssetLoader<Texture2D>::operator()(AssetLoaderContext const& loaderContext,
	std::string const& assetPath)
{
	auto filePath = MakeAssetFilePath(loaderContext, assetPath);
	
	std::ifstream stream(filePath, std::ios::binary);
	if (stream.fail()) {
		POMDOG_THROW_EXCEPTION(std::invalid_argument, "Cannot open file: " + filePath);
	}
	
	constexpr auto SignatureLength = sizeof(std::uint8_t) * 8;
	
	auto const fileSize = BinaryReader::GetBinarySize(stream);
	if (fileSize < SignatureLength) {
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented.");
	}

	auto signature = BinaryReader::ReadArray<std::uint8_t, 8>(stream);
	if (stream.fail()) {
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented.");
	}
	
	if (IsPNGFormat(signature)) {
		POMDOG_ASSERT(stream.tellg() == SignatureLength);
		
		std::vector<std::uint8_t> bufferWithoutSignature(fileSize - SignatureLength);
		stream.read(reinterpret_cast<char*>(bufferWithoutSignature.data()), bufferWithoutSignature.size());
		
		ImageBinary binary;
		binary.Data = bufferWithoutSignature.data();
		binary.ByteLength = bufferWithoutSignature.size();
		
		auto parsingData = ReadPNG(binary);
		
		auto graphicsDevice = loaderContext.GraphicsDevice.lock();
		POMDOG_ASSERT(graphicsDevice);
		
		auto texture = std::make_shared<Texture2D>(graphicsDevice, parsingData.Width, parsingData.Height, 1, parsingData.Format);
		texture->SetData(parsingData.Binary.data());
		return std::move(texture);
	}
	else {
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented.");
	}

	POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented.");
}
}// namespace Details
}// namespace Pomdog
