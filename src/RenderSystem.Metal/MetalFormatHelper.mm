// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#include "MetalFormatHelper.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"

namespace Pomdog {
namespace Detail {
namespace Metal {

Optional<MTLPixelFormat> MetalFormatHelper::ToMTLPixelFormat(SurfaceFormat format) noexcept
{
    switch (format) {
    case SurfaceFormat::A8_UNorm: return MTLPixelFormatA8Unorm;
    case SurfaceFormat::B8G8R8A8_UNorm: return MTLPixelFormatBGRA8Unorm;
    case SurfaceFormat::R10G10B10A2_UNorm: return MTLPixelFormatRGB10A2Unorm;
    case SurfaceFormat::R16G16B16A16_Float: return MTLPixelFormatRGBA16Float;
    case SurfaceFormat::R16G16_Float: return MTLPixelFormatRG16Float;
    case SurfaceFormat::R32G32B32A32_Float: return MTLPixelFormatRGBA32Float;
    case SurfaceFormat::R32_Float: return MTLPixelFormatR32Float;
    case SurfaceFormat::R8G8B8A8_UNorm: return MTLPixelFormatRGBA8Unorm;
    case SurfaceFormat::R8G8_UNorm: return MTLPixelFormatRG8Unorm;
    case SurfaceFormat::R8_UNorm: return MTLPixelFormatR8Unorm;

#if defined(MAC_OS_X_VERSION_10_11) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_11)
    case SurfaceFormat::BlockComp1_UNorm: return MTLPixelFormatBC1_RGBA;
    case SurfaceFormat::BlockComp2_UNorm: return MTLPixelFormatBC2_RGBA;
    case SurfaceFormat::BlockComp3_UNorm: return MTLPixelFormatBC3_RGBA;
#else
    // Not supported:
    case SurfaceFormat::BlockComp1_UNorm: break;
    case SurfaceFormat::BlockComp2_UNorm: break;
    case SurfaceFormat::BlockComp3_UNorm: break;
#endif
    // Not supported:
    case SurfaceFormat::R11G11B10_Float: break;
    }

    return Pomdog::NullOpt;
}

} // namespace Metal
} // namespace Detail
} // namespace Pomdog
