// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "RenderTarget2DMetal.hpp"
#include "MetalFormatHelper.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#import <Metal/MTLDevice.h>
#import <Metal/MTLTexture.h>

namespace Pomdog::Detail::Metal {

RenderTarget2DMetal::RenderTarget2DMetal(
    id<MTLDevice> device,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    SurfaceFormat format,
    DepthFormat depthStencilFormat,
    std::int32_t multiSampleCount)
    : texture(nil)
    , depthStencilTexture(nil)
    //, multiSampleEnabled(multiSampleCount > 1)
{
    POMDOG_ASSERT(device != nil);
    {
        MTLTextureDescriptor* descriptor = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:ToPixelFormat(format)
            width:pixelWidth
            height:pixelHeight
            mipmapped:(levelCount > 1 ? YES: NO)];

        [descriptor setUsage:MTLTextureUsageRenderTarget|MTLTextureUsageShaderRead];
//        [descriptor setStorageMode:];
//        [descriptor setResourceOptions:];
//        [descriptor setSampleCount:];
//        [descriptor setMipmapLevelCount:];

        texture = [device newTextureWithDescriptor:descriptor];
        if (texture == nil) {
            // FUS RO DAH!
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to create MTLTexture");
        }
    }

#if defined(DEBUG) && !defined(NDEBUG)
    if ((depthStencilFormat == DepthFormat::Depth24Stencil8) && !device.isDepth24Stencil8PixelFormatSupported) {
        // NOTE: MTLPixelFormatDepth24Unorm_Stencil8 is only supported in certain devices.
        Log::Warning("Pomdog", "This device does not support MTLPixelFormatDepth24Unorm_Stencil8.");
    }
#endif

    if (depthStencilFormat != DepthFormat::None) {
        MTLTextureDescriptor* descriptor = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:ToPixelFormat(depthStencilFormat)
            width:pixelWidth
            height:pixelHeight
            mipmapped:(levelCount > 1 ? YES: NO)];

        MTLResourceOptions resourceOptions = 0;
        resourceOptions |= MTLResourceStorageModePrivate;

        [descriptor setUsage:MTLTextureUsageRenderTarget];
        [descriptor setResourceOptions:resourceOptions];

        depthStencilTexture = [device newTextureWithDescriptor:descriptor];
        if (depthStencilTexture == nil) {
            // FUS RO DAH!
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to create MTLTexture");
        }
    }
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
