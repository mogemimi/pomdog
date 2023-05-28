// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/metal/texture2d_metal.h"
#include "pomdog/gpu/backends/surface_format_helper.h"
#include "pomdog/gpu/backends/texture_helper.h"
#include "pomdog/gpu/metal/metal_format_helper.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/utility/assert.h"
#import <Metal/MTLDevice.h>
#include <algorithm>

namespace pomdog::gpu::detail::metal {

std::unique_ptr<Error>
Texture2DMetal::Initialize(
    id<MTLDevice> device,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    std::int32_t levelCountIn,
    PixelFormat formatIn) noexcept
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
        return errors::make("failed to create MTLTexture");
    }
    return nullptr;
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

PixelFormat Texture2DMetal::GetFormat() const noexcept
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

        [texture replaceRegion:region
                   mipmapLevel:mipmapLevel
                     withBytes:reinterpret_cast<const std::uint8_t*>(pixelData) + startOffset
                   bytesPerRow:bytesPerRow];

        auto strideBytesPerMipmap = mipmapWidth * mipmapHeight * bytesPerPixel;
        switch (format) {
        case PixelFormat::BlockComp1_UNorm:
        case PixelFormat::BlockComp2_UNorm:
        case PixelFormat::BlockComp3_UNorm:
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

} // namespace pomdog::gpu::detail::metal
