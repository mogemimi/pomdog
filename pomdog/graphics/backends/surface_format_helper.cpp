// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "SurfaceFormatHelper.hpp"
#include "../Basic/Unreachable.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"

namespace Pomdog::Detail {

int SurfaceFormatHelper::ToBytesPerBlock(SurfaceFormat format) noexcept
{
    switch (format) {
    case SurfaceFormat::Invalid:
        return 0;
    case SurfaceFormat::A8_UNorm:
    case SurfaceFormat::R8_UNorm:
        return 1;
    case SurfaceFormat::R8G8_UNorm:
    case SurfaceFormat::BlockComp1_UNorm:
    case SurfaceFormat::Depth16:
        return 2;
    case SurfaceFormat::R8G8B8A8_UNorm:
    case SurfaceFormat::B8G8R8A8_UNorm:
    case SurfaceFormat::R10G10B10A2_UNorm:
    case SurfaceFormat::R11G11B10_Float:
    case SurfaceFormat::R16G16_Float:
    case SurfaceFormat::R32_Float:
    case SurfaceFormat::BlockComp2_UNorm:
    case SurfaceFormat::BlockComp3_UNorm:
    case SurfaceFormat::Depth24Stencil8:
    case SurfaceFormat::Depth32:
        return 4;
    case SurfaceFormat::Depth32_Float_Stencil8_Uint:
        return 5;
    case SurfaceFormat::R16G16B16A16_Float:
        return 8;
    case SurfaceFormat::R32G32B32A32_Float:
        return 16;
    }
    POMDOG_UNREACHABLE("Unsupported surface format");
}

} // namespace Pomdog::Detail
