// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/graphics/dxgi/dxgi_format_helper.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/graphics/index_element_size.h"
#include "pomdog/graphics/input_element_format.h"
#include "pomdog/graphics/surface_format.h"

namespace pomdog::detail::dxgi {

DXGI_FORMAT ToDXGIFormat(SurfaceFormat format) noexcept
{
    switch (format) {
    case SurfaceFormat::Invalid:
        return DXGI_FORMAT_UNKNOWN;
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
    case SurfaceFormat::Depth16:
        return DXGI_FORMAT_D16_UNORM;
    case SurfaceFormat::Depth32:
        return DXGI_FORMAT_D32_FLOAT;
    case SurfaceFormat::Depth24Stencil8:
        return DXGI_FORMAT_D24_UNORM_S8_UINT;
    case SurfaceFormat::Depth32_Float_Stencil8_Uint:
        return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    }
    POMDOG_UNREACHABLE("Unsupported surface format");
}

DXGI_FORMAT ToDXGIFormat(IndexElementSize elementSize) noexcept
{
    switch (elementSize) {
    case IndexElementSize::SixteenBits:
        return DXGI_FORMAT_R16_UINT;
    case IndexElementSize::ThirtyTwoBits:
        return DXGI_FORMAT_R32_UINT;
    }
    POMDOG_UNREACHABLE("Unsupported index element size");
}

DXGI_FORMAT ToDXGIFormat(InputElementFormat format) noexcept
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
    POMDOG_UNREACHABLE("Unsupported input element format");
}

} // namespace pomdog::detail::dxgi
