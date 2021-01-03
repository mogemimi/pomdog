// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/MagicaVoxel/VoxModelExporter.hpp"
#include "VoxChunkHeader.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Content/Utility/MakeFourCC.hpp"
#include "Pomdog/Experimental/MagicaVoxel/VoxModel.hpp"
#include "Pomdog/Utility/Assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <fstream>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::MagicaVoxel::VoxModelExporter {

[[nodiscard]] std::unique_ptr<Error>
Export(const VoxModel& model, const std::string& filePath) noexcept
{
    using Detail::MakeFourCC;

    constexpr std::int32_t MagicaVoxelVersion = 150;
    constexpr auto fourCC = MakeFourCC('V', 'O', 'X', ' ');
    constexpr auto IdMain = MakeFourCC('M', 'A', 'I', 'N');
    constexpr auto IdSize = MakeFourCC('S', 'I', 'Z', 'E');
    constexpr auto IdXYZI = MakeFourCC('X', 'Y', 'Z', 'I');
    constexpr auto IdRGBA = MakeFourCC('R', 'G', 'B', 'A');

    struct Size final {
        std::int32_t X;
        std::int32_t Y;
        std::int32_t Z;
    };

    Size size;
    size.X = model.X;
    size.Y = model.Y;
    size.Z = model.Z;

    std::int32_t const voxelCount = static_cast<std::int32_t>(model.Voxels.size());
    POMDOG_ASSERT(voxelCount >= 0);

    std::vector<MagicaVoxel::Voxel> voxels = model.Voxels;

    std::array<Color, 256> colors = model.ColorPalette;

    POMDOG_ASSERT(model.ColorPalette.size() == 256);
    std::rotate(
        std::begin(colors),
        std::next(std::begin(colors), 1),
        std::end(colors));

    POMDOG_ASSERT(colors.back() == Color::Black);

    using MagicaVoxel::VoxChunkHeader;

    VoxChunkHeader sizeChunk;
    sizeChunk.ID = IdSize;
    sizeChunk.ContentSize = sizeof(size);
    sizeChunk.ChildrenSize = 0;

    VoxChunkHeader xyziChunk;
    xyziChunk.ID = IdXYZI;
    const auto xyziChunkContentSize = sizeof(voxelCount) + (sizeof(MagicaVoxel::Voxel) * voxels.size());
    xyziChunk.ContentSize = static_cast<std::int32_t>(xyziChunkContentSize);
    xyziChunk.ChildrenSize = 0;

    POMDOG_ASSERT(xyziChunk.ContentSize > 0);

    VoxChunkHeader rgbaChunk;
    rgbaChunk.ID = IdRGBA;
    rgbaChunk.ContentSize = static_cast<int32_t>(sizeof(Color) * colors.size());
    rgbaChunk.ChildrenSize = 0;

    VoxChunkHeader mainChunk;
    mainChunk.ID = IdMain;
    mainChunk.ContentSize = 0;
    mainChunk.ChildrenSize =
        sizeof(sizeChunk) + sizeChunk.ContentSize +
        sizeof(xyziChunk) + xyziChunk.ContentSize +
        sizeof(rgbaChunk) + rgbaChunk.ContentSize;

    std::ofstream stream(filePath, std::ios::binary);

    if (stream.fail()) {
        return Errors::New("cannot open file: " + filePath);
    }

    stream.write(reinterpret_cast<const char*>(&fourCC), sizeof(fourCC));
    stream.write(reinterpret_cast<const char*>(&MagicaVoxelVersion), sizeof(MagicaVoxelVersion));
    stream.write(reinterpret_cast<const char*>(&mainChunk), sizeof(mainChunk));
    stream.write(reinterpret_cast<const char*>(&sizeChunk), sizeof(sizeChunk));
    stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
    stream.write(reinterpret_cast<const char*>(&xyziChunk), sizeof(xyziChunk));
    stream.write(reinterpret_cast<const char*>(&voxelCount), sizeof(voxelCount));
    stream.write(reinterpret_cast<const char*>(voxels.data()), sizeof(voxels.front()) * voxels.size());
    stream.write(reinterpret_cast<const char*>(&rgbaChunk), sizeof(rgbaChunk));
    stream.write(reinterpret_cast<const char*>(colors.data()), sizeof(colors.front()) * colors.size());

    return nullptr;
}

} // namespace Pomdog::MagicaVoxel::VoxModelExporter
