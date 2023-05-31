// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/backends/surface_format_helper.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/pixel_format.h"

namespace pomdog::gpu::detail::SurfaceFormatHelper {

[[nodiscard]] int toBytesPerBlock(PixelFormat format) noexcept
{
    switch (format) {
    case PixelFormat::Invalid:
        return 0;
    case PixelFormat::A8_UNorm:
    case PixelFormat::R8_UNorm:
        return 1;
    case PixelFormat::R8G8_UNorm:
    case PixelFormat::BlockComp1_UNorm:
    case PixelFormat::Depth16:
        return 2;
    case PixelFormat::R8G8B8A8_UNorm:
    case PixelFormat::B8G8R8A8_UNorm:
    case PixelFormat::R10G10B10A2_UNorm:
    case PixelFormat::R11G11B10_Float:
    case PixelFormat::R16G16_Float:
    case PixelFormat::R32_Float:
    case PixelFormat::BlockComp2_UNorm:
    case PixelFormat::BlockComp3_UNorm:
    case PixelFormat::Depth24Stencil8:
    case PixelFormat::Depth32:
        return 4;
    case PixelFormat::Depth32_Float_Stencil8_Uint:
        return 5;
    case PixelFormat::R16G16B16A16_Float:
        return 8;
    case PixelFormat::R32G32B32A32_Float:
        return 16;
    }
    POMDOG_UNREACHABLE("Unsupported surface format");
}

} // namespace pomdog::gpu::detail::SurfaceFormatHelper
