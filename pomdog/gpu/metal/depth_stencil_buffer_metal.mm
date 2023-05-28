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
DepthStencilBufferMetal::initialize(
    id<MTLDevice> device,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    PixelFormat depthStencilFormatIn,
    std::int32_t multiSampleCount) noexcept
{
    pixelWidth_ = pixelWidthIn;
    pixelHeight_ = pixelHeightIn;
    depthStencilFormat_ = depthStencilFormatIn;
    multiSampleEnabled_ = (multiSampleCount > 1);

    POMDOG_ASSERT(device != nullptr);

    if ((depthStencilFormat_ == PixelFormat::Depth24Stencil8) && !device.isDepth24Stencil8PixelFormatSupported) {
        // NOTE: MTLPixelFormatDepth24Unorm_Stencil8 is only supported in certain devices.
        return errors::make("This device does not support MTLPixelFormatDepth24Unorm_Stencil8.");
    }

    if (depthStencilFormat_ == PixelFormat::Invalid) {
        return errors::make("depthStencilFormat must be != PixelFormat::None");
    }

    MTLTextureDescriptor* descriptor = [MTLTextureDescriptor
        texture2DDescriptorWithPixelFormat:ToPixelFormat(depthStencilFormat_)
                                     width:pixelWidth_
                                    height:pixelHeight_
                                 mipmapped:NO];

    MTLResourceOptions resourceOptions = 0;
    resourceOptions |= MTLResourceStorageModePrivate;

    [descriptor setUsage:MTLTextureUsageRenderTarget];
    [descriptor setResourceOptions:resourceOptions];

    depthStencilTexture_ = [device newTextureWithDescriptor:descriptor];
    if (depthStencilTexture_ == nullptr) {
        return errors::make("failed to create MTLTexture");
    }

    return nullptr;
}

std::int32_t DepthStencilBufferMetal::getWidth() const noexcept
{
    return pixelWidth_;
}

std::int32_t DepthStencilBufferMetal::getHeight() const noexcept
{
    return pixelHeight_;
}

PixelFormat DepthStencilBufferMetal::getFormat() const noexcept
{
    return depthStencilFormat_;
}

Rectangle DepthStencilBufferMetal::getBounds() const noexcept
{
    return Rectangle{0, 0, pixelWidth_, pixelHeight_};
}

id<MTLTexture> DepthStencilBufferMetal::getTexture() const noexcept
{
    return depthStencilTexture_;
}

} // namespace pomdog::gpu::detail::metal
