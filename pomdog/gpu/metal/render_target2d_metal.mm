// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/metal/render_target2d_metal.h"
#include "pomdog/gpu/backends/surface_format_helper.h"
#include "pomdog/gpu/metal/metal_format_helper.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/utility/assert.h"
#import <Metal/MTLDevice.h>
#import <Metal/MTLTexture.h>

namespace pomdog::gpu::detail::metal {

std::unique_ptr<Error>
RenderTarget2DMetal::initialize(
    id<MTLDevice> device,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    std::int32_t levelCountIn,
    PixelFormat formatIn,
    std::int32_t multiSampleCount) noexcept
{
    pixelWidth_ = pixelWidthIn;
    pixelHeight_ = pixelHeightIn;
    levelCount_ = levelCountIn;
    format_ = formatIn;
    multiSampleEnabled_ = (multiSampleCount > 1);

    POMDOG_ASSERT(device != nullptr);

    MTLTextureDescriptor* descriptor = [MTLTextureDescriptor
        texture2DDescriptorWithPixelFormat:ToPixelFormat(format_)
                                     width:pixelWidth_
                                    height:pixelHeight_
                                 mipmapped:(levelCount_ > 1 ? YES : NO)];

    [descriptor setUsage:MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead];
    // [descriptor setStorageMode:];
    // [descriptor setResourceOptions:];
    // [descriptor setSampleCount:];
    // [descriptor setMipmapLevelCount:];

    texture_ = [device newTextureWithDescriptor:descriptor];
    if (texture_ == nullptr) {
        return errors::make("failed to create MTLTexture");
    }

    return nullptr;
}

std::int32_t RenderTarget2DMetal::getWidth() const noexcept
{
    return pixelWidth_;
}

std::int32_t RenderTarget2DMetal::getHeight() const noexcept
{
    return pixelHeight_;
}

std::int32_t RenderTarget2DMetal::getLevelCount() const noexcept
{
    return levelCount_;
}

PixelFormat RenderTarget2DMetal::getFormat() const noexcept
{
    return format_;
}

Rectangle RenderTarget2DMetal::getBounds() const noexcept
{
    return Rectangle{0, 0, pixelWidth_, pixelHeight_};
}

void RenderTarget2DMetal::getData(
    void* result,
    std::size_t offsetInBytes,
    std::size_t sizeInBytes) const
{
    POMDOG_ASSERT(texture_ != nullptr);
    POMDOG_ASSERT(result != nullptr);

    const auto bytesPerPixel = SurfaceFormatHelper::ToBytesPerBlock(format_);

    // FIXME: Not implemented yet.
    POMDOG_ASSERT(offsetInBytes == 0);
    POMDOG_ASSERT(sizeInBytes == static_cast<std::size_t>(bytesPerPixel * pixelWidth_ * pixelHeight_));
    MTLRegion region = MTLRegionMake2D(0, 0, pixelWidth_, pixelHeight_);

    // NOTE: Don't use getBytes() for textures with MTLResourceStorageModePrivate.
    [texture_ getBytes:result bytesPerRow:(bytesPerPixel * pixelWidth_) fromRegion:region mipmapLevel:0];
}

id<MTLTexture> RenderTarget2DMetal::getTexture() const noexcept
{
    return texture_;
}

} // namespace pomdog::gpu::detail::metal
