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
RenderTarget2DMetal::Initialize(
    id<MTLDevice> device,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    std::int32_t levelCountIn,
    SurfaceFormat formatIn,
    std::int32_t multiSampleCount) noexcept
{
    pixelWidth = pixelWidthIn;
    pixelHeight = pixelHeightIn;
    levelCount = levelCountIn;
    format = formatIn;
    multiSampleEnabled = (multiSampleCount > 1);

    POMDOG_ASSERT(device != nullptr);

    MTLTextureDescriptor* descriptor = [MTLTextureDescriptor
        texture2DDescriptorWithPixelFormat:ToPixelFormat(format)
                                     width:pixelWidth
                                    height:pixelHeight
                                 mipmapped:(levelCount > 1 ? YES : NO)];

    [descriptor setUsage:MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead];
    // [descriptor setStorageMode:];
    // [descriptor setResourceOptions:];
    // [descriptor setSampleCount:];
    // [descriptor setMipmapLevelCount:];

    texture = [device newTextureWithDescriptor:descriptor];
    if (texture == nullptr) {
        return errors::New("failed to create MTLTexture");
    }

    return nullptr;
}

std::int32_t RenderTarget2DMetal::GetWidth() const noexcept
{
    return pixelWidth;
}

std::int32_t RenderTarget2DMetal::GetHeight() const noexcept
{
    return pixelHeight;
}

std::int32_t RenderTarget2DMetal::GetLevelCount() const noexcept
{
    return levelCount;
}

SurfaceFormat RenderTarget2DMetal::GetFormat() const noexcept
{
    return format;
}

Rectangle RenderTarget2DMetal::GetBounds() const noexcept
{
    return Rectangle{0, 0, pixelWidth, pixelHeight};
}

void RenderTarget2DMetal::GetData(
    void* result,
    std::size_t offsetInBytes,
    std::size_t sizeInBytes) const
{
    POMDOG_ASSERT(texture != nullptr);
    POMDOG_ASSERT(result != nullptr);

    auto const bytesPerPixel = SurfaceFormatHelper::ToBytesPerBlock(format);

    // FIXME: Not implemented yet.
    POMDOG_ASSERT(offsetInBytes == 0);
    POMDOG_ASSERT(sizeInBytes == static_cast<std::size_t>(bytesPerPixel * pixelWidth * pixelHeight));
    MTLRegion region = MTLRegionMake2D(0, 0, pixelWidth, pixelHeight);

    // NOTE: Don't use getBytes() for textures with MTLResourceStorageModePrivate.
    [texture getBytes:result bytesPerRow:(bytesPerPixel * pixelWidth) fromRegion:region mipmapLevel:0];
}

id<MTLTexture> RenderTarget2DMetal::GetTexture() const noexcept
{
    return texture;
}

} // namespace pomdog::gpu::detail::metal
