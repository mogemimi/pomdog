// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Texture2DMetal.hpp"
#include "MetalFormatHelper.hpp"
#include "../RenderSystem/SurfaceFormatHelper.hpp"
#include "../RenderSystem/TextureHelper.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#import <Metal/MTLDevice.h>
#include <algorithm>

namespace Pomdog::Detail::Metal {

Texture2DMetal::Texture2DMetal(
    id<MTLDevice> device,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    std::int32_t levelCountIn,
    SurfaceFormat formatIn)
{
    POMDOG_ASSERT(device != nullptr);

    texture = nullptr;
    pixelWidth = pixelWidthIn;
    pixelHeight = pixelHeightIn;
    levelCount = levelCountIn;
    format = formatIn;

    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);

    MTLTextureDescriptor* descriptor = [MTLTextureDescriptor
        texture2DDescriptorWithPixelFormat:ToPixelFormat(format)
                                     width:pixelWidth
                                    height:pixelHeight
                                 mipmapped:(levelCount > 1 ? YES : NO)];

    texture = [device newTextureWithDescriptor:descriptor];
    if (texture == nullptr) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create MTLTexture");
    }
}

std::int32_t Texture2DMetal::GetWidth() const noexcept
{
    return pixelWidth;
}

std::int32_t Texture2DMetal::GetHeight() const noexcept
{
    return pixelHeight;
}

std::int32_t Texture2DMetal::GetLevelCount() const noexcept
{
    return levelCount;
}

SurfaceFormat Texture2DMetal::GetFormat() const noexcept
{
    return format;
}

void Texture2DMetal::SetData(const void* pixelData)
{
    POMDOG_ASSERT(texture != nullptr);
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);
    POMDOG_ASSERT(pixelData != nullptr);

    auto const bytesPerPixel = SurfaceFormatHelper::ToBytesPerBlock(format);

    auto mipmapWidth = pixelWidth;
    auto mipmapHeight = pixelHeight;
    std::size_t startOffset = 0;

    for (int mipmapLevel = 0; mipmapLevel < levelCount; ++mipmapLevel) {
        MTLRegion region = MTLRegionMake2D(0, 0, mipmapWidth, mipmapHeight);

        auto bytesPerRow = mipmapWidth * bytesPerPixel;
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

        [texture replaceRegion:region
                   mipmapLevel:mipmapLevel
                     withBytes:reinterpret_cast<const std::uint8_t*>(pixelData) + startOffset
                   bytesPerRow:bytesPerRow];

        auto strideBytesPerMipmap = mipmapWidth * mipmapHeight * bytesPerPixel;
        switch (format) {
        case SurfaceFormat::BlockComp1_UNorm:
        case SurfaceFormat::BlockComp2_UNorm:
        case SurfaceFormat::BlockComp3_UNorm:
            strideBytesPerMipmap = bytesPerRow * (std::max(mipmapHeight, 4) / 4);
            break;
        default:
            break;
        }

        startOffset += strideBytesPerMipmap;
        mipmapWidth = std::max((mipmapWidth >> 1), 1);
        mipmapHeight = std::max((mipmapHeight >> 1), 1);
    }
}

id<MTLTexture> Texture2DMetal::GetTexture() const noexcept
{
    return texture;
}

} // namespace Pomdog::Detail::Metal
