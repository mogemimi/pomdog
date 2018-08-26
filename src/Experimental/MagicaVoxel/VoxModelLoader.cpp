// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/MagicaVoxel/VoxModelLoader.hpp"
#include "VoxChunkHeader.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Content/Utility/BinaryReader.hpp"
#include "Pomdog/Content/Utility/MakeFourCC.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <algorithm>
#include <fstream>
#include <utility>

namespace Pomdog {
namespace MagicaVoxel {
namespace {

std::string Error(const std::string& assetName, const std::string& description)
{
    return description + (": " + assetName);
}

std::ifstream::pos_type ChunkSize(std::ifstream& stream, const VoxChunkHeader& chunk)
{
    POMDOG_ASSERT(chunk.ContentSize >= 0);
    POMDOG_ASSERT(chunk.ChildrenSize >= 0);
    POMDOG_ASSERT(stream.tellg() >= 0);

    return stream.tellg() + static_cast<std::ifstream::pos_type>(chunk.ContentSize + chunk.ChildrenSize);
}

} // unnamed namespace

VoxModel VoxModelLoader::Load(const AssetManager& assets, const std::string& assetName)
{
    using Detail::BinaryReader;
    using Detail::MakeFourCC;

    constexpr std::int32_t MagicaVoxelVersion = 150;
    constexpr auto fourCC = MakeFourCC('V', 'O', 'X', ' ');
    constexpr auto IdMain = MakeFourCC('M', 'A', 'I', 'N');
    constexpr auto IdSize = MakeFourCC('S', 'I', 'Z', 'E');
    constexpr auto IdXYZI = MakeFourCC('X', 'Y', 'Z', 'I');
    constexpr auto IdRGBA = MakeFourCC('R', 'G', 'B', 'A');

    auto binaryFile = assets.OpenStream(assetName);
    auto& stream = binaryFile.Stream;

    if (stream.fail()) {
        POMDOG_THROW_EXCEPTION(std::invalid_argument,
            Error(assetName, "cannot open file"));
    }

    if (fourCC != BinaryReader::Read<std::uint32_t>(stream)) {
        POMDOG_THROW_EXCEPTION(std::invalid_argument,
            Error(assetName, "invalid format"));
    }

    if (MagicaVoxelVersion != BinaryReader::Read<std::int32_t>(stream)) {
        POMDOG_THROW_EXCEPTION(std::invalid_argument,
            Error(assetName, "version does not much"));
    }

    const auto mainChunk = BinaryReader::Read<VoxChunkHeader>(stream);

    if (mainChunk.ID != IdMain) {
        POMDOG_THROW_EXCEPTION(std::invalid_argument,
            Error(assetName, "cannot find main chunk"));
    }

    const auto mainChunkEnd = ChunkSize(stream, mainChunk);

    stream.seekg(mainChunk.ContentSize, std::ios::cur);

    MagicaVoxel::VoxModel model;

    while (stream.tellg() < mainChunkEnd) {
        const auto chunk = BinaryReader::Read<VoxChunkHeader>(stream);
        const auto chunkEnd = ChunkSize(stream, chunk);

        if (chunk.ID == IdSize) {
            model.X = BinaryReader::Read<std::int32_t>(stream);
            model.Y = BinaryReader::Read<std::int32_t>(stream);
            model.Z = BinaryReader::Read<std::int32_t>(stream);

            POMDOG_ASSERT(model.X >= 0);
            POMDOG_ASSERT(model.Y >= 0);
            POMDOG_ASSERT(model.Z >= 0);

            if (model.X < 0 || model.Y < 0 || model.Z < 0) {
                POMDOG_THROW_EXCEPTION(std::invalid_argument,
                    Error(assetName, "invalid format"));
            }
        }
        else if (chunk.ID == IdXYZI) {
            const auto voxelCount = BinaryReader::Read<std::int32_t>(stream);
            if (voxelCount < 0) {
                POMDOG_THROW_EXCEPTION(std::invalid_argument,
                    Error(assetName, "negative number of voxels"));
            }

            if (voxelCount > 0) {
                model.Voxels = BinaryReader::ReadArray<MagicaVoxel::Voxel>(stream, voxelCount);
            }
        }
        else if (chunk.ID == IdRGBA) {
            model.ColorPalette = BinaryReader::Read<decltype(model.ColorPalette)>(stream);

            model.ColorPalette.back() = Color::Black;

            POMDOG_ASSERT(model.ColorPalette.size() == 256);
            std::rotate(
                std::begin(model.ColorPalette),
                std::next(std::begin(model.ColorPalette), 255),
                std::end(model.ColorPalette));
        }

        stream.seekg(chunkEnd, std::ios::beg);
    }

    return model;
}

} // namespace MagicaVoxel
} // namespace Pomdog
