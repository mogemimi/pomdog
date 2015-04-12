// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "TextureHelper.hpp"
#include "SurfaceFormatHelper.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>

namespace Pomdog {
namespace Detail {
namespace {

static std::size_t MipmapImageDataBytes(
    std::size_t pixelWidth,
    std::size_t pixelHeight,
    std::size_t bytesPerBlock) noexcept
{
    return pixelWidth * pixelHeight * bytesPerBlock;
}

} // unnamed namespace
//-----------------------------------------------------------------------
std::int32_t TextureHelper::ComputeMipmapLevelCount(std::int32_t width, std::int32_t height)
{
    POMDOG_ASSERT(width >= 0);
    POMDOG_ASSERT(height >= 0);

    auto size = std::max(width, height);
    std::int32_t levelCount = 1;

    POMDOG_ASSERT(size >= 0);

    while (size > 1)
    {
        size = size / 2;
        ++levelCount;
    }
    return levelCount;
}
//-----------------------------------------------------------------------
std::int32_t TextureHelper::ComputeTextureSizeInBytes(
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    SurfaceFormat format)
{
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);

    using RenderSystem::SurfaceFormatHelper;
    auto const bytesPerBlock = SurfaceFormatHelper::ToBytesPerBlock(format);

    std::size_t sizeInBytes = 0;
    std::size_t mipMapPixelWidth = pixelWidth;
    std::size_t mipMapPixelHeight = pixelHeight;

    for (int mipmapLevel = 0; mipmapLevel < levelCount; ++mipmapLevel)
    {
        sizeInBytes += MipmapImageDataBytes(pixelWidth, pixelHeight, bytesPerBlock);
        mipMapPixelWidth = std::max((mipMapPixelWidth >> 1), 1U);
        mipMapPixelHeight = std::max((mipMapPixelHeight >> 1), 1U);
    }

    return sizeInBytes;
}
//-----------------------------------------------------------------------
}// namespace Detail
}// namespace Pomdog
