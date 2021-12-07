// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/graphics/metal/depth_stencil_buffer_metal.hpp"
#include "pomdog/graphics/backends/surface_format_helper.hpp"
#include "pomdog/graphics/metal/metal_format_helper.hpp"
#include "pomdog/math/rectangle.hpp"
#include "pomdog/utility/assert.hpp"
#import <Metal/MTLDevice.h>
#import <Metal/MTLTexture.h>

namespace Pomdog::Detail::Metal {

std::unique_ptr<Error>
DepthStencilBufferMetal::Initialize(
    id<MTLDevice> device,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    SurfaceFormat depthStencilFormatIn,
    std::int32_t multiSampleCount) noexcept
{
    pixelWidth = pixelWidthIn;
    pixelHeight = pixelHeightIn;
    depthStencilFormat = depthStencilFormatIn;
    multiSampleEnabled = (multiSampleCount > 1);

    POMDOG_ASSERT(device != nullptr);

    if ((depthStencilFormat == SurfaceFormat::Depth24Stencil8) && !device.isDepth24Stencil8PixelFormatSupported) {
        // NOTE: MTLPixelFormatDepth24Unorm_Stencil8 is only supported in certain devices.
        return Errors::New("This device does not support MTLPixelFormatDepth24Unorm_Stencil8.");
    }

    if (depthStencilFormat == SurfaceFormat::Invalid) {
        return Errors::New("depthStencilFormat must be != SurfaceFormat::None");
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
        return Errors::New("failed to create MTLTexture");
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

SurfaceFormat DepthStencilBufferMetal::GetFormat() const noexcept
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

} // namespace Pomdog::Detail::Metal
