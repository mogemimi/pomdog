// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/magicavoxel/vox_model_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/content/utility/binary_reader.h"
#include "pomdog/content/utility/make_fourcc.h"
#include "pomdog/experimental/magicavoxel/vox_chunk_header.h"
#include "pomdog/experimental/magicavoxel/vox_model.h"
#include "pomdog/utility/assert.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <sstream>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

namespace BinaryReader = detail::BinaryReader;
using detail::makeFourCC;
using magicavoxel::VoxChunkHeader;

template <typename Stream>
std::tuple<std::shared_ptr<magicavoxel::VoxModel>, std::unique_ptr<Error>>
parseVoxModel(Stream& stream, const std::string& filePath)
{
    constexpr std::int32_t MagicaVoxelVersion = 150;
    constexpr auto fourCC = makeFourCC('V', 'O', 'X', ' ');
    constexpr auto IdMain = makeFourCC('M', 'A', 'I', 'N');
    constexpr auto IdSize = makeFourCC('S', 'I', 'Z', 'E');
    constexpr auto IdXYZI = makeFourCC('X', 'Y', 'Z', 'I');
    constexpr auto IdRGBA = makeFourCC('R', 'G', 'B', 'A');

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

    const auto mainChunkEnd = stream.tellg() + static_cast<typename Stream::pos_type>(mainChunk.ContentSize + mainChunk.ChildrenSize);

    stream.seekg(mainChunk.ContentSize, std::ios::cur);

    auto model = std::make_shared<magicavoxel::VoxModel>();

    while (stream.tellg() < mainChunkEnd) {
        const auto chunk = BinaryReader::read<VoxChunkHeader>(stream);
        const auto chunkEnd = stream.tellg() + static_cast<typename Stream::pos_type>(chunk.ContentSize + chunk.ChildrenSize);

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

} // namespace

std::tuple<std::shared_ptr<magicavoxel::VoxModel>, std::unique_ptr<Error>>
loadVoxModel(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::string& filePath)
{
    auto [file, openErr] = vfs::open(fs, filePath);
    if (openErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(openErr), "cannot open vox file, " + filePath));
    }

    auto [info, statErr] = file->stat();
    if (statErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(statErr), "cannot stat vox file, " + filePath));
    }

    std::vector<u8> buffer(static_cast<std::size_t>(info.size));
    auto [bytesRead, readErr] = file->read(std::span<u8>(buffer));
    if (readErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(readErr), "cannot read vox file, " + filePath));
    }

    std::istringstream stream(std::string(buffer.begin(), buffer.end()), std::ios::binary);
    return parseVoxModel(stream, filePath);
}

} // namespace pomdog
