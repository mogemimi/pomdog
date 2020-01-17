// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "DXGIFormatHelper.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Graphics/IndexElementSize.hpp"
#include "Pomdog/Graphics/InputElementFormat.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog::Detail::DXGI {

DXGI_FORMAT DXGIFormatHelper::ToDXGIFormat(DepthFormat format) noexcept
{
    POMDOG_ASSERT(format != DepthFormat::None);
    switch (format) {
    case DepthFormat::Depth16:
        return DXGI_FORMAT_D16_UNORM;
    case DepthFormat::Depth32:
        return DXGI_FORMAT_D32_FLOAT;
    case DepthFormat::Depth24Stencil8:
        return DXGI_FORMAT_D24_UNORM_S8_UINT;
    case DepthFormat::Depth32_Float_Stencil8_Uint:
        return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    case DepthFormat::None:
        break;
    }
    return DXGI_FORMAT_UNKNOWN;
}

DXGI_FORMAT DXGIFormatHelper::ToDXGIFormat(SurfaceFormat format) noexcept
{
    switch (format) {
    case SurfaceFormat::A8_UNorm:
        return DXGI_FORMAT_A8_UNORM;
    case SurfaceFormat::B8G8R8A8_UNorm:
        return DXGI_FORMAT_B8G8R8A8_UNORM;
    case SurfaceFormat::BlockComp1_UNorm:
        return DXGI_FORMAT_BC1_UNORM;
    case SurfaceFormat::BlockComp2_UNorm:
        return DXGI_FORMAT_BC2_UNORM;
    case SurfaceFormat::BlockComp3_UNorm:
        return DXGI_FORMAT_BC3_UNORM;
    case SurfaceFormat::R10G10B10A2_UNorm:
        return DXGI_FORMAT_R10G10B10A2_UNORM;
    case SurfaceFormat::R11G11B10_Float:
        return DXGI_FORMAT_R11G11B10_FLOAT;
    case SurfaceFormat::R16G16B16A16_Float:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case SurfaceFormat::R16G16_Float:
        return DXGI_FORMAT_R16G16_FLOAT;
    case SurfaceFormat::R32G32B32A32_Float:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case SurfaceFormat::R32_Float:
        return DXGI_FORMAT_R32_FLOAT;
    case SurfaceFormat::R8G8B8A8_UNorm:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case SurfaceFormat::R8G8_UNorm:
        return DXGI_FORMAT_R8G8_UNORM;
    case SurfaceFormat::R8_UNorm:
        return DXGI_FORMAT_R8_UNORM;
    }
    return DXGI_FORMAT_UNKNOWN;
}

DXGI_FORMAT DXGIFormatHelper::ToDXGIFormat(IndexElementSize elementSize) noexcept
{
    switch (elementSize) {
    case IndexElementSize::SixteenBits:
        return DXGI_FORMAT_R16_UINT;
    case IndexElementSize::ThirtyTwoBits:
        return DXGI_FORMAT_R32_UINT;
    }
    return DXGI_FORMAT_UNKNOWN;
}

DXGI_FORMAT DXGIFormatHelper::ToDXGIFormat(InputElementFormat format) noexcept
{
    switch (format) {
    case InputElementFormat::Byte4:
        return DXGI_FORMAT_R8G8B8A8_UINT;
    case InputElementFormat::Float:
        return DXGI_FORMAT_R32_FLOAT;
    case InputElementFormat::Float2:
        return DXGI_FORMAT_R32G32_FLOAT;
    case InputElementFormat::Float3:
        return DXGI_FORMAT_R32G32B32_FLOAT;
    case InputElementFormat::Float4:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case InputElementFormat::HalfFloat2:
        return DXGI_FORMAT_R16G16_FLOAT;
    case InputElementFormat::HalfFloat4:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case InputElementFormat::Int4:
        return DXGI_FORMAT_R32G32B32A32_SINT;
    }
    return DXGI_FORMAT_R32_FLOAT;
}

} // namespace Pomdog::Detail::DXGI
