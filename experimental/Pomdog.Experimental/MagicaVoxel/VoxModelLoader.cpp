// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "VoxModelLoader.hpp"
#include "detail/VoxChunkHeader.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <fstream>
#include <algorithm>
#include <utility>

///@todo badcode
#include <Pomdog/../../src/Content/Utility/MakeFourCC.hpp>
#include <Pomdog/../../src/Content/Utility/BinaryReader.hpp>

namespace Pomdog {
namespace MagicaVoxel {
namespace {

static std::string Error(std::string const& assetName, char const* description)
{
	return description + (": " + assetName);
}
//-----------------------------------------------------------------------
static std::ifstream::pos_type ChunkSize(std::ifstream & stream, MagicaVoxel::VoxChunkHeader const& chunk)
{
	POMDOG_ASSERT(chunk.ContentSize >= 0);
	POMDOG_ASSERT(chunk.ChildrenSize >= 0);
	POMDOG_ASSERT(stream.tellg() >= 0);

	return stream.tellg() + static_cast<std::ifstream::pos_type>(chunk.ContentSize + chunk.ChildrenSize);
}

}// unnamed namespace
//-----------------------------------------------------------------------
VoxModel VoxModelLoader::Load(AssetManager const& assets, std::string const& assetName)
{
	using MagicaVoxel::VoxChunkHeader;
	using Detail::BinaryReader;
	using Detail::MakeFourCC;

	constexpr std::int32_t MagicaVoxelVersion = 150;
	constexpr auto fourCC = MakeFourCC('V', 'O', 'X', ' ');
	constexpr auto IdMain = MakeFourCC('M', 'A', 'I', 'N');
	constexpr auto IdSize = MakeFourCC('S', 'I', 'Z', 'E');
	constexpr auto IdXYZI = MakeFourCC('X', 'Y', 'Z', 'I');
	constexpr auto IdRGBA = MakeFourCC('R', 'G', 'B', 'A');

	std::ifstream stream = assets.OpenStream(assetName);

	if (stream.fail()) {
		POMDOG_THROW_EXCEPTION(std::invalid_argument, Error(assetName, "cannot open file"));
	}

	if (fourCC != BinaryReader::Read<std::uint32_t>(stream)) {
		POMDOG_THROW_EXCEPTION(std::invalid_argument, Error(assetName, "invalid format"));
	}

	if (MagicaVoxelVersion != BinaryReader::Read<std::int32_t>(stream)) {
		POMDOG_THROW_EXCEPTION(std::invalid_argument, Error(assetName, "version does not much"));
	}

	const auto mainChunk = BinaryReader::Read<VoxChunkHeader>(stream);

	if (mainChunk.ID != IdMain) {
		POMDOG_THROW_EXCEPTION(std::invalid_argument, Error(assetName, "cannot find main chunk"));
	}

	const auto mainChunkEnd = ChunkSize(stream, mainChunk);

	stream.seekg(mainChunk.ContentSize, std::ios::cur);

	MagicaVoxel::VoxModel model;

	while (stream.tellg() < mainChunkEnd)
	{
		const auto chunk = BinaryReader::Read<VoxChunkHeader>(stream);
		const auto chunkEnd = ChunkSize(stream, chunk);

		if (chunk.ID == IdSize) {
			const auto x = BinaryReader::Read<std::int32_t>(stream);
			const auto y = BinaryReader::Read<std::int32_t>(stream);
			const auto z = BinaryReader::Read<std::int32_t>(stream);

			POMDOG_ASSERT(x >= 0);
			POMDOG_ASSERT(y >= 0);
			POMDOG_ASSERT(z >= 0);

			model.X = static_cast<std::uint32_t>(x);
			model.Y = static_cast<std::uint32_t>(y);
			model.Z = static_cast<std::uint32_t>(z);
		}
		else if (chunk.ID == IdXYZI) {
			const auto voxelCount = BinaryReader::Read<std::int32_t>(stream);
			if (voxelCount < 0) {
				POMDOG_THROW_EXCEPTION(std::invalid_argument, Error(assetName, "negative number of voxels"));
			}

			if (voxelCount > 0) {
				model.Voxels = BinaryReader::ReadArray<MagicaVoxel::Voxel>(stream, voxelCount);
			}
		}
		else if (chunk.ID == IdRGBA) {
			model.ColorPalette = BinaryReader::Read<decltype(model.ColorPalette)>(stream);

			model.ColorPalette.back() = Color::Black;

			POMDOG_ASSERT(model.ColorPalette.size() == 256);
			std::rotate(std::begin(model.ColorPalette), std::next(std::begin(model.ColorPalette), 255), std::end(model.ColorPalette));
		}

		stream.seekg(chunkEnd, std::ios::beg);
	}

	return std::move(model);
}
//-----------------------------------------------------------------------
}// namespace MagicaVoxel
}// namespace Pomdog
