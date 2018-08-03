// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Texture2DMetal.hpp"
#include "MetalFormatHelper.hpp"
#include "../RenderSystem/SurfaceFormatHelper.hpp"
#include "../RenderSystem/TextureHelper.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <algorithm>
#import <Metal/MTLDevice.h>

namespace Pomdog {
namespace Detail {
namespace Metal {
namespace {

float MipmapImageDataBytes(float width, float height, float bytesPerPixel)
{
    return width * height * bytesPerPixel;
}

} // unnamed namespace

Texture2DMetal::Texture2DMetal(
    id<MTLDevice> device,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    SurfaceFormat format)
    : texture(nil)
{
    POMDOG_ASSERT(device != nil);

    MTLTextureDescriptor* descriptor = [MTLTextureDescriptor
        texture2DDescriptorWithPixelFormat:ToPixelFormat(format)
        width:pixelWidth
        height:pixelHeight
        mipmapped:(levelCount > 1 ? YES: NO)];

    texture = [device newTextureWithDescriptor:descriptor];
    if (texture == nil) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create MTLTexture");
    }
}

void Texture2DMetal::SetData(
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    SurfaceFormat format,
    const void* pixelData)
{
    POMDOG_ASSERT(texture != nil);
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);
    POMDOG_ASSERT(pixelData != nullptr);

    POMDOG_ASSERT(format != SurfaceFormat::BlockComp1_UNorm
        && format != SurfaceFormat::BlockComp2_UNorm
        && format != SurfaceFormat::BlockComp3_UNorm);

    auto const bytesPerPixel = SurfaceFormatHelper::ToBytesPerBlock(format);

    auto mipMapPixelWidth = pixelWidth;
    auto mipMapPixelHeight = pixelHeight;
    std::size_t startOffset = 0;

    for (int mipmapLevel = 0; mipmapLevel < levelCount; ++mipmapLevel) {
        MTLRegion region = MTLRegionMake2D(0, 0, mipMapPixelWidth, mipMapPixelHeight);

        const auto bytesPerRow = mipMapPixelWidth * bytesPerPixel;

        [texture replaceRegion:region
            mipmapLevel:mipmapLevel
            withBytes:reinterpret_cast<const std::uint8_t*>(pixelData) + startOffset
            bytesPerRow:bytesPerRow];

        const auto strideBytesPerMipmap = MipmapImageDataBytes(
            mipMapPixelWidth, mipMapPixelHeight, bytesPerPixel);

        startOffset += strideBytesPerMipmap;
        mipMapPixelWidth = std::max((mipMapPixelWidth >> 1), 1);
        mipMapPixelHeight = std::max((mipMapPixelHeight >> 1), 1);
    }
}

id<MTLTexture> Texture2DMetal::GetTexture() const noexcept
{
    return texture;
}

} // namespace Metal
} // namespace Detail
} // namespace Pomdog
