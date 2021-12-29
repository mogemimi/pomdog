// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/backends/texture_helper.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/surface_format_helper.h"
#include "pomdog/gpu/surface_format.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail {

std::int32_t TextureHelper::ComputeMipmapLevelCount(std::int32_t width, std::int32_t height)
{
    POMDOG_ASSERT(width >= 0);
    POMDOG_ASSERT(height >= 0);

    auto size = std::max(width, height);
    std::int32_t levelCount = 1;

    POMDOG_ASSERT(size >= 0);

    while (size > 1) {
        size = size / 2;
        ++levelCount;
    }
    return levelCount;
}

std::int32_t TextureHelper::ComputeTextureSizeInBytes(
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    SurfaceFormat format)
{
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);

    const auto bytesPerBlock = SurfaceFormatHelper::ToBytesPerBlock(format);

    std::int32_t totalBytes = 0;
    std::int32_t mipmapWidth = pixelWidth;
    std::int32_t mipmapHeight = pixelHeight;

    for (int mipmapLevel = 0; mipmapLevel < levelCount; ++mipmapLevel) {
        auto bytesPerRow = mipmapWidth * bytesPerBlock;
        switch (format) {
        case SurfaceFormat::BlockComp1_UNorm:
            bytesPerRow = 8 * (std::max(mipmapWidth, 4) / 4);
            break;
        case SurfaceFormat::BlockComp2_UNorm:
        case SurfaceFormat::BlockComp3_UNorm:
            bytesPerRow = 16 * (std::max(mipmapWidth, 4) / 4);
            break;
        default:
            break;
        }

        auto strideBytesPerMipmap = bytesPerRow * mipmapHeight;
        switch (format) {
        case SurfaceFormat::BlockComp1_UNorm:
        case SurfaceFormat::BlockComp2_UNorm:
        case SurfaceFormat::BlockComp3_UNorm:
            strideBytesPerMipmap = bytesPerRow * (std::max(mipmapHeight, 4) / 4);
            break;
        default:
            break;
        }

        totalBytes += strideBytesPerMipmap;

        mipmapWidth = std::max((mipmapWidth >> 1), 1);
        mipmapHeight = std::max((mipmapHeight >> 1), 1);
    }

    return totalBytes;
}

} // namespace pomdog::gpu::detail
