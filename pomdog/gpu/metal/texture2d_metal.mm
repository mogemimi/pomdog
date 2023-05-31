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
Texture2DMetal::initialize(
    id<MTLDevice> device,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    std::int32_t levelCountIn,
    PixelFormat formatIn) noexcept
{
    POMDOG_ASSERT(device != nullptr);

    texture_ = nullptr;
    pixelWidth_ = pixelWidthIn;
    pixelHeight_ = pixelHeightIn;
    levelCount_ = levelCountIn;
    format_ = formatIn;

    POMDOG_ASSERT(pixelWidth_ > 0);
    POMDOG_ASSERT(pixelHeight_ > 0);
    POMDOG_ASSERT(levelCount_ >= 1);

    MTLTextureDescriptor* descriptor = [MTLTextureDescriptor
        texture2DDescriptorWithPixelFormat:ToPixelFormat(format_)
                                     width:pixelWidth_
                                    height:pixelHeight_
                                 mipmapped:(levelCount_ > 1 ? YES : NO)];

    texture_ = [device newTextureWithDescriptor:descriptor];
    if (texture_ == nullptr) {
        return errors::make("failed to create MTLTexture");
    }
    return nullptr;
}

std::int32_t Texture2DMetal::getWidth() const noexcept
{
    return pixelWidth_;
}

std::int32_t Texture2DMetal::getHeight() const noexcept
{
    return pixelHeight_;
}

std::int32_t Texture2DMetal::getLevelCount() const noexcept
{
    return levelCount_;
}

PixelFormat Texture2DMetal::getFormat() const noexcept
{
    return format_;
}

void Texture2DMetal::setData(const void* pixelData)
{
    POMDOG_ASSERT(texture_ != nullptr);
    POMDOG_ASSERT(pixelWidth_ > 0);
    POMDOG_ASSERT(pixelHeight_ > 0);
    POMDOG_ASSERT(levelCount_ >= 1);
    POMDOG_ASSERT(pixelData != nullptr);

    auto const bytesPerPixel = SurfaceFormatHelper::toBytesPerBlock(format_);

    auto mipmapWidth = pixelWidth_;
    auto mipmapHeight = pixelHeight_;
    std::size_t startOffset = 0;

    for (int mipmapLevel = 0; mipmapLevel < levelCount_; ++mipmapLevel) {
        MTLRegion region = MTLRegionMake2D(0, 0, mipmapWidth, mipmapHeight);

        auto bytesPerRow = mipmapWidth * bytesPerPixel;
        switch (format_) {
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

        [texture_ replaceRegion:region
                    mipmapLevel:mipmapLevel
                      withBytes:reinterpret_cast<const std::uint8_t*>(pixelData) + startOffset
                    bytesPerRow:bytesPerRow];

        auto strideBytesPerMipmap = mipmapWidth * mipmapHeight * bytesPerPixel;
        switch (format_) {
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

id<MTLTexture> Texture2DMetal::getTexture() const noexcept
{
    return texture_;
}

} // namespace pomdog::gpu::detail::metal
