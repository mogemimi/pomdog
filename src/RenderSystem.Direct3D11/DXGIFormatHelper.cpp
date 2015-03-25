// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "DXGIFormatHelper.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {
//-----------------------------------------------------------------------
DXGI_FORMAT DXGIFormatHelper::ToDXGIFormat(DepthFormat format)
{
    POMDOG_ASSERT(format != DepthFormat::None);

    switch (format) {
    case DepthFormat::Depth16: return DXGI_FORMAT_D16_UNORM;
    case DepthFormat::Depth32: return DXGI_FORMAT_D32_FLOAT;
    case DepthFormat::Depth24Stencil8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
    case DepthFormat::None:
        break;
    };

    return DXGI_FORMAT_UNKNOWN;
}
//-----------------------------------------------------------------------
DXGI_FORMAT DXGIFormatHelper::ToDXGIFormat(SurfaceFormat format)
{
    switch (format) {
    case SurfaceFormat::A8_UNorm: return DXGI_FORMAT_A8_UNORM;
    case SurfaceFormat::B8G8R8A8_UNorm: return DXGI_FORMAT_B8G8R8A8_UNORM;
    case SurfaceFormat::BlockComp1_UNorm: return DXGI_FORMAT_BC1_UNORM;
    case SurfaceFormat::BlockComp2_UNorm: return DXGI_FORMAT_BC2_UNORM;
    case SurfaceFormat::BlockComp3_UNorm: return DXGI_FORMAT_BC3_UNORM;
    case SurfaceFormat::R10G10B10A2_UNorm: return DXGI_FORMAT_R10G10B10A2_UNORM;
    case SurfaceFormat::R11G11B10_Float: return DXGI_FORMAT_R11G11B10_FLOAT;
    case SurfaceFormat::R16G16B16A16_Float: return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case SurfaceFormat::R16G16_Float: return DXGI_FORMAT_R16G16_FLOAT;
    case SurfaceFormat::R32G32B32A32_Float: return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case SurfaceFormat::R32_Float: return DXGI_FORMAT_R32_FLOAT;
    case SurfaceFormat::R8G8B8A8_UNorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
    case SurfaceFormat::R8G8_UNorm: return DXGI_FORMAT_R8G8_UNORM;
    case SurfaceFormat::R8_UNorm: return DXGI_FORMAT_R8_UNORM;
    };

    return DXGI_FORMAT_UNKNOWN;
}
//-----------------------------------------------------------------------
}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog
