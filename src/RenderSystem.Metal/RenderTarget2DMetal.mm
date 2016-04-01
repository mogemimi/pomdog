// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "RenderTarget2DMetal.hpp"
#include "MetalFormatHelper.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#import <Metal/MTLDevice.h>
#import <Metal/MTLTexture.h>

namespace Pomdog {
namespace Detail {
namespace Metal {
namespace {

MTLPixelFormat ToMTLPixelFormat(DepthFormat depthFormat)
{
    POMDOG_ASSERT(depthFormat != DepthFormat::None);
    POMDOG_ASSERT_MESSAGE(depthFormat != DepthFormat::Depth16, "Not supported");

    switch (depthFormat) {
    case DepthFormat::Depth16: return MTLPixelFormatDepth32Float;
    case DepthFormat::Depth32: return MTLPixelFormatDepth32Float;
#if defined(MAC_OS_X_VERSION_10_11) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_11)
    case DepthFormat::Depth24Stencil8: return MTLPixelFormatDepth24Unorm_Stencil8;
#else
    case DepthFormat::Depth24Stencil8: return MTLPixelFormatDepth32Float_Stencil8;
#endif
    case DepthFormat::None: return MTLPixelFormatInvalid;
    }
    return MTLPixelFormatDepth32Float;
}

} // unnamed namespace

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
        auto pixelFormat = MetalFormatHelper::ToMTLPixelFormat(format);

        if (!pixelFormat) {
            // FUS RO DAH!
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "This platform does not support this pixel format.");
        }

        MTLTextureDescriptor* descriptor = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:*pixelFormat
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

    if (depthStencilFormat != DepthFormat::None)
    {
        MTLTextureDescriptor* descriptor = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:ToMTLPixelFormat(depthStencilFormat)
            width:pixelWidth
            height:pixelHeight
            mipmapped:(levelCount > 1 ? YES: NO)];

        depthStencilTexture = [device newTextureWithDescriptor:descriptor];
        if (depthStencilTexture == nil) {
            // FUS RO DAH!
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to create MTLTexture");
        }
    }
}
//-----------------------------------------------------------------------
id<MTLTexture> RenderTarget2DMetal::GetTexture() const noexcept
{
    return texture;
}

} // namespace Metal
} // namespace Detail
} // namespace Pomdog
