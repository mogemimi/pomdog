// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/magicavoxel/vox_model_loader.hpp"
#include "pomdog/application/file_system.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/content/asset_manager.hpp"
#include "pomdog/content/utility/binary_reader.hpp"
#include "pomdog/content/utility/make_fourcc.hpp"
#include "pomdog/experimental/magicavoxel/vox_chunk_header.hpp"
#include "pomdog/experimental/magicavoxel/vox_model.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <fstream>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {
namespace {

using Detail::BinaryReader;
using Detail::MakeFourCC;
using MagicaVoxel::VoxChunkHeader;

std::ifstream::pos_type ChunkSize(std::ifstream& stream, const VoxChunkHeader& chunk)
{
    POMDOG_ASSERT(chunk.ContentSize >= 0);
    POMDOG_ASSERT(chunk.ChildrenSize >= 0);
    POMDOG_ASSERT(stream.tellg() >= 0);

    return stream.tellg() + static_cast<std::ifstream::pos_type>(chunk.ContentSize + chunk.ChildrenSize);
}

} // namespace

std::tuple<std::shared_ptr<MagicaVoxel::VoxModel>, std::unique_ptr<Error>>
AssetLoader<MagicaVoxel::VoxModel>::operator()([[maybe_unused]] AssetManager& assets, const std::string& filePath)
{
    constexpr std::int32_t MagicaVoxelVersion = 150;
    constexpr auto fourCC = MakeFourCC('V', 'O', 'X', ' ');
    constexpr auto IdMain = MakeFourCC('M', 'A', 'I', 'N');
    constexpr auto IdSize = MakeFourCC('S', 'I', 'Z', 'E');
    constexpr auto IdXYZI = MakeFourCC('X', 'Y', 'Z', 'I');
    constexpr auto IdRGBA = MakeFourCC('R', 'G', 'B', 'A');

    std::ifstream stream{filePath, std::ifstream::binary};

    if (!stream) {
        auto err = Errors::New("cannot open the file, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    auto [byteLength, sizeErr] = FileSystem::GetFileSize(filePath);
    if (sizeErr != nullptr) {
        auto err = Errors::Wrap(std::move(sizeErr), "failed to get file size, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    POMDOG_ASSERT(stream);

    if (byteLength <= 0) {
        auto err = Errors::New("the font file is too small " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    if (fourCC != BinaryReader::Read<std::uint32_t>(stream)) {
        auto err = Errors::New("invalid VOX format " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    if (MagicaVoxelVersion != BinaryReader::Read<std::int32_t>(stream)) {
        auto err = Errors::New("version does not much " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    const auto mainChunk = BinaryReader::Read<VoxChunkHeader>(stream);

    if (mainChunk.ID != IdMain) {
        auto err = Errors::New("cannot find main chunk " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    const auto mainChunkEnd = ChunkSize(stream, mainChunk);

    stream.seekg(mainChunk.ContentSize, std::ios::cur);

    auto model = std::make_shared<MagicaVoxel::VoxModel>();

    while (stream.tellg() < mainChunkEnd) {
        const auto chunk = BinaryReader::Read<VoxChunkHeader>(stream);
        const auto chunkEnd = ChunkSize(stream, chunk);

        if (chunk.ID == IdSize) {
            model->X = BinaryReader::Read<std::int32_t>(stream);
            model->Y = BinaryReader::Read<std::int32_t>(stream);
            model->Z = BinaryReader::Read<std::int32_t>(stream);

            POMDOG_ASSERT(model->X >= 0);
            POMDOG_ASSERT(model->Y >= 0);
            POMDOG_ASSERT(model->Z >= 0);

            if (model->X < 0 || model->Y < 0 || model->Z < 0) {
                auto err = Errors::New("invalid VOX format " + filePath);
                return std::make_tuple(nullptr, std::move(err));
            }
        }
        else if (chunk.ID == IdXYZI) {
            const auto voxelCount = BinaryReader::Read<std::int32_t>(stream);
            if (voxelCount < 0) {
                auto err = Errors::New("negative number of voxels " + filePath);
                return std::make_tuple(nullptr, std::move(err));
            }

            if (voxelCount > 0) {
                model->Voxels = BinaryReader::ReadArray<MagicaVoxel::Voxel>(stream, voxelCount);
            }
        }
        else if (chunk.ID == IdRGBA) {
            model->ColorPalette = BinaryReader::Read<decltype(model->ColorPalette)>(stream);

            model->ColorPalette.back() = Color::Black;

            POMDOG_ASSERT(model->ColorPalette.size() == 256);
            std::rotate(
                std::begin(model->ColorPalette),
                std::next(std::begin(model->ColorPalette), 255),
                std::end(model->ColorPalette));
        }

        stream.seekg(chunkEnd, std::ios::beg);
    }

    return std::make_tuple(std::move(model), nullptr);
}

} // namespace Pomdog::Detail
