// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "MetalFormatHelper.hpp"
#include "../Basic/Unreachable.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"

namespace Pomdog {
namespace Detail {
namespace Metal {

MTLPixelFormat ToPixelFormat(SurfaceFormat format) noexcept
{
    switch (format) {
    case SurfaceFormat::A8_UNorm: return MTLPixelFormatA8Unorm;
    case SurfaceFormat::B8G8R8A8_UNorm: return MTLPixelFormatBGRA8Unorm;
    case SurfaceFormat::R10G10B10A2_UNorm: return MTLPixelFormatRGB10A2Unorm;
    case SurfaceFormat::R16G16B16A16_Float: return MTLPixelFormatRGBA16Float;
    case SurfaceFormat::R11G11B10_Float: return MTLPixelFormatRG11B10Float;
    case SurfaceFormat::R16G16_Float: return MTLPixelFormatRG16Float;
    case SurfaceFormat::R32G32B32A32_Float: return MTLPixelFormatRGBA32Float;
    case SurfaceFormat::R32_Float: return MTLPixelFormatR32Float;
    case SurfaceFormat::R8G8B8A8_UNorm: return MTLPixelFormatRGBA8Unorm;
    case SurfaceFormat::R8G8_UNorm: return MTLPixelFormatRG8Unorm;
    case SurfaceFormat::R8_UNorm: return MTLPixelFormatR8Unorm;
    case SurfaceFormat::BlockComp1_UNorm: return MTLPixelFormatBC1_RGBA;
    case SurfaceFormat::BlockComp2_UNorm: return MTLPixelFormatBC2_RGBA;
    case SurfaceFormat::BlockComp3_UNorm: return MTLPixelFormatBC3_RGBA;
    }
    POMDOG_UNREACHABLE("Unsupported surface format");
}

MTLPixelFormat ToPixelFormat(DepthFormat depthFormat) noexcept
{
    switch (depthFormat) {
#if defined(MAC_OS_X_VERSION_10_12) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_12)
    case DepthFormat::Depth16: return MTLPixelFormatDepth16Unorm;
#else
    case DepthFormat::Depth16: return MTLPixelFormatDepth32Float;
#endif
    case DepthFormat::Depth32: return MTLPixelFormatDepth32Float;
    case DepthFormat::Depth24Stencil8: return MTLPixelFormatDepth24Unorm_Stencil8;
    case DepthFormat::Depth32_Float_Stencil8_Uint: return MTLPixelFormatDepth32Float_Stencil8;
    case DepthFormat::None: return MTLPixelFormatInvalid;
    }
    POMDOG_UNREACHABLE("Unsupported depth format");
}

} // namespace Metal
} // namespace Detail
} // namespace Pomdog
