// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/magicavoxel/vox_model_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/asset_manager.h"
#include "pomdog/content/utility/binary_reader.h"
#include "pomdog/content/utility/make_fourcc.h"
#include "pomdog/experimental/magicavoxel/vox_chunk_header.h"
#include "pomdog/experimental/magicavoxel/vox_model.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <fstream>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
namespace {

namespace BinaryReader = detail::BinaryReader;
using detail::makeFourCC;
using magicavoxel::VoxChunkHeader;

std::ifstream::pos_type ChunkSize(std::ifstream& stream, const VoxChunkHeader& chunk)
{
    POMDOG_ASSERT(chunk.ContentSize >= 0);
    POMDOG_ASSERT(chunk.ChildrenSize >= 0);
    POMDOG_ASSERT(stream.tellg() >= 0);

    return stream.tellg() + static_cast<std::ifstream::pos_type>(chunk.ContentSize + chunk.ChildrenSize);
}

} // namespace

std::tuple<std::shared_ptr<magicavoxel::VoxModel>, std::unique_ptr<Error>>
AssetLoader<magicavoxel::VoxModel>::operator()([[maybe_unused]] AssetManager& assets, const std::string& filePath)
{
    constexpr std::int32_t MagicaVoxelVersion = 150;
    constexpr auto fourCC = makeFourCC('V', 'O', 'X', ' ');
    constexpr auto IdMain = makeFourCC('M', 'A', 'I', 'N');
    constexpr auto IdSize = makeFourCC('S', 'I', 'Z', 'E');
    constexpr auto IdXYZI = makeFourCC('X', 'Y', 'Z', 'I');
    constexpr auto IdRGBA = makeFourCC('R', 'G', 'B', 'A');

    std::ifstream stream{filePath, std::ifstream::binary};

    if (!stream) {
        auto err = errors::make("cannot open the file, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    auto [byteLength, sizeErr] = FileSystem::getFileSize(filePath);
    if (sizeErr != nullptr) {
        auto err = errors::wrap(std::move(sizeErr), "failed to get file size, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    POMDOG_ASSERT(stream);

    if (byteLength <= 0) {
        auto err = errors::make("the font file is too small " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    if (fourCC != BinaryReader::read<std::uint32_t>(stream)) {
        auto err = errors::make("invalid VOX format " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    if (MagicaVoxelVersion != BinaryReader::read<std::int32_t>(stream)) {
        auto err = errors::make("version does not much " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    const auto mainChunk = BinaryReader::read<VoxChunkHeader>(stream);

    if (mainChunk.ID != IdMain) {
        auto err = errors::make("cannot find main chunk " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    const auto mainChunkEnd = ChunkSize(stream, mainChunk);

    stream.seekg(mainChunk.ContentSize, std::ios::cur);

    auto model = std::make_shared<magicavoxel::VoxModel>();

    while (stream.tellg() < mainChunkEnd) {
        const auto chunk = BinaryReader::read<VoxChunkHeader>(stream);
        const auto chunkEnd = ChunkSize(stream, chunk);

        if (chunk.ID == IdSize) {
            model->X = BinaryReader::read<std::int32_t>(stream);
            model->Y = BinaryReader::read<std::int32_t>(stream);
            model->Z = BinaryReader::read<std::int32_t>(stream);

            POMDOG_ASSERT(model->X >= 0);
            POMDOG_ASSERT(model->Y >= 0);
            POMDOG_ASSERT(model->Z >= 0);

            if (model->X < 0 || model->Y < 0 || model->Z < 0) {
                auto err = errors::make("invalid VOX format " + filePath);
                return std::make_tuple(nullptr, std::move(err));
            }
        }
        else if (chunk.ID == IdXYZI) {
            const auto voxelCount = BinaryReader::read<std::int32_t>(stream);
            if (voxelCount < 0) {
                auto err = errors::make("negative number of voxels " + filePath);
                return std::make_tuple(nullptr, std::move(err));
            }

            if (voxelCount > 0) {
                model->Voxels = BinaryReader::readArray<magicavoxel::Voxel>(stream, voxelCount);
            }
        }
        else if (chunk.ID == IdRGBA) {
            model->ColorPalette = BinaryReader::read<decltype(model->ColorPalette)>(stream);

            model->ColorPalette.back() = Color::createBlack();

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

} // namespace pomdog::detail
