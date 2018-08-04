// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "TextureHelper.hpp"
#include "SurfaceFormatHelper.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>

namespace Pomdog {
namespace Detail {
namespace {

std::int32_t MipmapImageDataBytes(
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t bytesPerBlock) noexcept
{
    return pixelWidth * pixelHeight * bytesPerBlock;
}

} // unnamed namespace

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

    auto const bytesPerBlock = SurfaceFormatHelper::ToBytesPerBlock(format);

    std::int32_t sizeInBytes = 0;
    std::int32_t mipMapWidth = pixelWidth;
    std::int32_t mipMapHeight = pixelHeight;

    for (int mipmapLevel = 0; mipmapLevel < levelCount; ++mipmapLevel) {
        sizeInBytes += MipmapImageDataBytes(
            mipMapWidth, mipMapHeight, bytesPerBlock);

        mipMapWidth = std::max((mipMapWidth >> 1), 1);
        mipMapHeight = std::max((mipMapHeight >> 1), 1);
    }

    return sizeInBytes;
}

} // namespace Detail
} // namespace Pomdog
