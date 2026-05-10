// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/backends/texture_helper.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/surface_format_helper.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::TextureHelper {

[[nodiscard]] i32
computeMipmapLevelCount(i32 width, i32 height)
{
    POMDOG_ASSERT(width >= 0);
    POMDOG_ASSERT(height >= 0);

    auto size = std::max(width, height);
    i32 levelCount = 1;

    POMDOG_ASSERT(size >= 0);

    while (size > 1) {
        size = size / 2;
        ++levelCount;
    }
    return levelCount;
}

[[nodiscard]] i32
computeTextureSizeInBytes(
    i32 pixelWidth,
    i32 pixelHeight,
    i32 levelCount,
    PixelFormat format)
{
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);

    const auto bytesPerBlock = SurfaceFormatHelper::toBytesPerBlock(format);

    i32 totalBytes = 0;
    i32 mipmapWidth = pixelWidth;
    i32 mipmapHeight = pixelHeight;

    for (i32 mipmapLevel = 0; mipmapLevel < levelCount; ++mipmapLevel) {
        auto bytesPerRow = mipmapWidth * bytesPerBlock;
        switch (format) {
        case PixelFormat::BlockComp1_UNorm:
            bytesPerRow = 8 * (std::max(mipmapWidth, 4) / 4);
            break;
        case PixelFormat::BlockComp2_UNorm:
        case PixelFormat::BlockComp3_UNorm:
            bytesPerRow = 16 * (std::max(mipmapWidth, 4) / 4);
            break;
        default:
            break;
        }

        auto strideBytesPerMipmap = bytesPerRow * mipmapHeight;
        switch (format) {
        case PixelFormat::BlockComp1_UNorm:
        case PixelFormat::BlockComp2_UNorm:
        case PixelFormat::BlockComp3_UNorm:
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

} // namespace pomdog::gpu::detail::TextureHelper
