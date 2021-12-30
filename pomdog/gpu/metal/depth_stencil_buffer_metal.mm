// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/metal/depth_stencil_buffer_metal.h"
#include "pomdog/gpu/backends/surface_format_helper.h"
#include "pomdog/gpu/metal/metal_format_helper.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/utility/assert.h"
#import <Metal/MTLDevice.h>
#import <Metal/MTLTexture.h>

namespace pomdog::gpu::detail::metal {

std::unique_ptr<Error>
DepthStencilBufferMetal::Initialize(
    id<MTLDevice> device,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    PixelFormat depthStencilFormatIn,
    std::int32_t multiSampleCount) noexcept
{
    pixelWidth = pixelWidthIn;
    pixelHeight = pixelHeightIn;
    depthStencilFormat = depthStencilFormatIn;
    multiSampleEnabled = (multiSampleCount > 1);

    POMDOG_ASSERT(device != nullptr);

    if ((depthStencilFormat == PixelFormat::Depth24Stencil8) && !device.isDepth24Stencil8PixelFormatSupported) {
        // NOTE: MTLPixelFormatDepth24Unorm_Stencil8 is only supported in certain devices.
        return errors::New("This device does not support MTLPixelFormatDepth24Unorm_Stencil8.");
    }

    if (depthStencilFormat == PixelFormat::Invalid) {
        return errors::New("depthStencilFormat must be != PixelFormat::None");
    }

    MTLTextureDescriptor* descriptor = [MTLTextureDescriptor
        texture2DDescriptorWithPixelFormat:ToPixelFormat(depthStencilFormat)
                                     width:pixelWidth
                                    height:pixelHeight
                                 mipmapped:NO];

    MTLResourceOptions resourceOptions = 0;
    resourceOptions |= MTLResourceStorageModePrivate;

    [descriptor setUsage:MTLTextureUsageRenderTarget];
    [descriptor setResourceOptions:resourceOptions];

    depthStencilTexture = [device newTextureWithDescriptor:descriptor];
    if (depthStencilTexture == nullptr) {
        return errors::New("failed to create MTLTexture");
    }

    return nullptr;
}

std::int32_t DepthStencilBufferMetal::GetWidth() const noexcept
{
    return pixelWidth;
}

std::int32_t DepthStencilBufferMetal::GetHeight() const noexcept
{
    return pixelHeight;
}

PixelFormat DepthStencilBufferMetal::GetFormat() const noexcept
{
    return depthStencilFormat;
}

Rectangle DepthStencilBufferMetal::GetBounds() const noexcept
{
    return Rectangle{0, 0, pixelWidth, pixelHeight};
}

id<MTLTexture> DepthStencilBufferMetal::GetTexture() const noexcept
{
    return depthStencilTexture;
}

} // namespace pomdog::gpu::detail::metal
