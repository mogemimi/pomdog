//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "VoxModelLoader.hpp"
#include "Pomdog/Content/detail/AssetLoaderContext.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <fstream>
#include <algorithm>
#include <utility>

///@todo badcode
#include <Pomdog/../../src/Content/Utility/MakeFourCC.hpp>
#include <Pomdog/../../src/Content/Utility/BinaryReader.hpp>

namespace Pomdog {
namespace Details {
namespace {
//-----------------------------------------------------------------------
static bool IsMagicaVoxelFormat(std::uint32_t signature)
{
	constexpr auto fourCC = MakeFourCC('V', 'O', 'X', ' ');
	return fourCC == signature;
}
//-----------------------------------------------------------------------
static std::string Error(std::string const& assetPath, char const* description)
{
	return description + (": " + assetPath);
}


struct Chunk {
	std::int32_t ID;
	std::int32_t ContentSize;
	std::int32_t ChildrenSize;
};

//-----------------------------------------------------------------------
static std::size_t ChunkSize(std::ifstream & stream, Chunk const& chunk)
{
	POMDOG_ASSERT(chunk.ContentSize >= 0);
	POMDOG_ASSERT(chunk.ChildrenSize >= 0);
	POMDOG_ASSERT(stream.tellg() >= 0);
	
	return static_cast<std::size_t>(stream.tellg()) + chunk.ContentSize + chunk.ChildrenSize;
}

//-----------------------------------------------------------------------
}// unnamed namespace
//-----------------------------------------------------------------------
MagicaVoxel::VoxModel AssetLoader<MagicaVoxel::VoxModel>::operator()(
	AssetLoaderContext const& loaderContext, std::string const& assetPath)
{
	constexpr int MagicaVoxelVersion = 150;
	constexpr auto IdMain = MakeFourCC('M', 'A', 'I', 'N');
	constexpr auto IdSize = MakeFourCC('S', 'I', 'Z', 'E');
	constexpr auto IdXYZI = MakeFourCC('X', 'Y', 'Z', 'I');
	constexpr auto IdRGBA = MakeFourCC('R', 'G', 'B', 'A');
	
	std::ifstream stream = loaderContext.OpenStream(assetPath);
	
	if (stream.fail()) {
		POMDOG_THROW_EXCEPTION(std::invalid_argument, Error(assetPath, "cannot open file"));
	}
	
	if (!IsMagicaVoxelFormat(BinaryReader::Read<std::uint32_t>(stream))) {
		POMDOG_THROW_EXCEPTION(std::invalid_argument, Error(assetPath, "invalid format"));
	}

	if (MagicaVoxelVersion != BinaryReader::Read<std::int32_t>(stream)) {
		POMDOG_THROW_EXCEPTION(std::invalid_argument, Error(assetPath, "version does not much"));
	}

	const auto mainChunk = BinaryReader::Read<Chunk>(stream);
	
	if (mainChunk.ID != IdMain) {
		POMDOG_THROW_EXCEPTION(std::invalid_argument, Error(assetPath, "cannot find main chunk"));
	}
	
	const auto mainChunkEnd = ChunkSize(stream, mainChunk);

	stream.seekg(mainChunk.ContentSize, std::ios::cur);
	
	MagicaVoxel::VoxModel model;

	while (stream.tellg() < mainChunkEnd)
	{
		const auto chunk = BinaryReader::Read<Chunk>(stream);
		const auto chunkEnd = ChunkSize(stream, chunk);

		switch (chunk.ID) {
		case IdSize: {
			const auto x = BinaryReader::Read<std::int32_t>(stream);
			const auto y = BinaryReader::Read<std::int32_t>(stream);
			const auto z = BinaryReader::Read<std::int32_t>(stream);
			
			POMDOG_ASSERT(x >= 0);
			POMDOG_ASSERT(y >= 0);
			POMDOG_ASSERT(z >= 0);
			
			model.X = static_cast<std::uint32_t>(x);
			model.Y = static_cast<std::uint32_t>(y);
			model.Z = static_cast<std::uint32_t>(z);
			break;
		}
		case IdXYZI: {
			const auto voxelCount = BinaryReader::Read<std::int32_t>(stream);
			if (voxelCount < 0) {
				POMDOG_THROW_EXCEPTION(std::invalid_argument, Error(assetPath, "negative number of voxels"));
			}
		
			if (voxelCount > 0) {
				model.Voxels = BinaryReader::ReadArray<MagicaVoxel::Voxel>(stream, voxelCount);
			}
			break;
		}
		case IdRGBA: {
			model.ColorPalette = BinaryReader::Read<decltype(model.ColorPalette)>(stream);

			model.ColorPalette.back() = Color::Black;
			
			POMDOG_ASSERT(model.ColorPalette.size() == 256);
			std::rotate(std::begin(model.ColorPalette), std::next(std::begin(model.ColorPalette), 255), std::end(model.ColorPalette));
			break;
		}
		default:
			break;
		}
		
		stream.seekg(chunkEnd, std::ios::beg);
	}

	return std::move(model);
}

}// namespace Details
}// namespace Pomdog
