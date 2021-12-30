// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/dxgi/dxgi_format_helper.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/index_element_size.h"
#include "pomdog/gpu/input_element_format.h"
#include "pomdog/gpu/pixel_format.h"

namespace pomdog::gpu::detail::dxgi {

DXGI_FORMAT ToDXGIFormat(PixelFormat format) noexcept
{
    switch (format) {
    case PixelFormat::Invalid:
        return DXGI_FORMAT_UNKNOWN;
    case PixelFormat::A8_UNorm:
        return DXGI_FORMAT_A8_UNORM;
    case PixelFormat::B8G8R8A8_UNorm:
        return DXGI_FORMAT_B8G8R8A8_UNORM;
    case PixelFormat::BlockComp1_UNorm:
        return DXGI_FORMAT_BC1_UNORM;
    case PixelFormat::BlockComp2_UNorm:
        return DXGI_FORMAT_BC2_UNORM;
    case PixelFormat::BlockComp3_UNorm:
        return DXGI_FORMAT_BC3_UNORM;
    case PixelFormat::R10G10B10A2_UNorm:
        return DXGI_FORMAT_R10G10B10A2_UNORM;
    case PixelFormat::R11G11B10_Float:
        return DXGI_FORMAT_R11G11B10_FLOAT;
    case PixelFormat::R16G16B16A16_Float:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case PixelFormat::R16G16_Float:
        return DXGI_FORMAT_R16G16_FLOAT;
    case PixelFormat::R32G32B32A32_Float:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case PixelFormat::R32_Float:
        return DXGI_FORMAT_R32_FLOAT;
    case PixelFormat::R8G8B8A8_UNorm:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case PixelFormat::R8G8_UNorm:
        return DXGI_FORMAT_R8G8_UNORM;
    case PixelFormat::R8_UNorm:
        return DXGI_FORMAT_R8_UNORM;
    case PixelFormat::Depth16:
        return DXGI_FORMAT_D16_UNORM;
    case PixelFormat::Depth32:
        return DXGI_FORMAT_D32_FLOAT;
    case PixelFormat::Depth24Stencil8:
        return DXGI_FORMAT_D24_UNORM_S8_UINT;
    case PixelFormat::Depth32_Float_Stencil8_Uint:
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

} // namespace pomdog::gpu::detail::dxgi
