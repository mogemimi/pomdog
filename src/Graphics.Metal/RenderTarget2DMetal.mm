// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "RenderTarget2DMetal.hpp"
#include "MetalFormatHelper.hpp"
#include "../Graphics.Backends/SurfaceFormatHelper.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Utility/Assert.hpp"
#import <Metal/MTLDevice.h>
#import <Metal/MTLTexture.h>

namespace Pomdog::Detail::Metal {

std::shared_ptr<Error>
RenderTarget2DMetal::Initialize(
    id<MTLDevice> device,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    std::int32_t levelCountIn,
    SurfaceFormat formatIn,
    DepthFormat depthStencilFormatIn,
    std::int32_t multiSampleCount) noexcept
{
    pixelWidth = pixelWidthIn;
    pixelHeight = pixelHeightIn;
    levelCount = levelCountIn;
    format = formatIn;
    depthStencilFormat = depthStencilFormatIn;
    multiSampleEnabled = (multiSampleCount > 1);

    POMDOG_ASSERT(device != nullptr);
    {
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
            return Errors::New("failed to create MTLTexture");
        }
    }

    if ((depthStencilFormat == DepthFormat::Depth24Stencil8) && !device.isDepth24Stencil8PixelFormatSupported) {
        // NOTE: MTLPixelFormatDepth24Unorm_Stencil8 is only supported in certain devices.
        return Errors::New("This device does not support MTLPixelFormatDepth24Unorm_Stencil8.");
    }

    if (depthStencilFormat != DepthFormat::None) {
        MTLTextureDescriptor* descriptor = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:ToPixelFormat(depthStencilFormat)
                                         width:pixelWidth
                                        height:pixelHeight
                                     mipmapped:(levelCount > 1 ? YES : NO)];

        MTLResourceOptions resourceOptions = 0;
        resourceOptions |= MTLResourceStorageModePrivate;

        [descriptor setUsage:MTLTextureUsageRenderTarget];
        [descriptor setResourceOptions:resourceOptions];

        depthStencilTexture = [device newTextureWithDescriptor:descriptor];
        if (depthStencilTexture == nullptr) {
            return Errors::New("failed to create MTLTexture");
        }
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

DepthFormat RenderTarget2DMetal::GetDepthStencilFormat() const noexcept
{
    return depthStencilFormat;
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

id<MTLTexture> RenderTarget2DMetal::GetDepthStencilTexture() const noexcept
{
    return depthStencilTexture;
}

} // namespace Pomdog::Detail::Metal
