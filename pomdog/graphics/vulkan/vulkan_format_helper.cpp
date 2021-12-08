// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/graphics/vulkan/vulkan_format_helper.hpp"
#include "pomdog/graphics/surface_format.hpp"
#include "pomdog/utility/assert.hpp"

namespace pomdog::detail::vulkan {

VkFormat
ToSurfaceFormat(SurfaceFormat format) noexcept
{
    switch (format) {
    case SurfaceFormat::Invalid:
        return VK_FORMAT_UNDEFINED;
    case SurfaceFormat::A8_UNorm:
        return VK_FORMAT_R8_UNORM;
    case SurfaceFormat::B8G8R8A8_UNorm:
        return VK_FORMAT_B8G8R8A8_UNORM;
    case SurfaceFormat::BlockComp1_UNorm:
        return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
    case SurfaceFormat::BlockComp2_UNorm:
        return VK_FORMAT_BC2_UNORM_BLOCK;
    case SurfaceFormat::BlockComp3_UNorm:
        return VK_FORMAT_BC3_UNORM_BLOCK;
    case SurfaceFormat::R10G10B10A2_UNorm:
        return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
    case SurfaceFormat::R11G11B10_Float:
        return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
    case SurfaceFormat::R16G16B16A16_Float:
        return VK_FORMAT_R16G16B16A16_SFLOAT;
    case SurfaceFormat::R16G16_Float:
        return VK_FORMAT_R16G16_SFLOAT;
    case SurfaceFormat::R32G32B32A32_Float:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case SurfaceFormat::R32_Float:
        return VK_FORMAT_R32_SFLOAT;
    case SurfaceFormat::R8G8B8A8_UNorm:
        return VK_FORMAT_R8G8B8A8_UNORM;
    case SurfaceFormat::R8G8_UNorm:
        return VK_FORMAT_R8G8_UNORM;
    case SurfaceFormat::R8_UNorm:
        return VK_FORMAT_R8_UNORM;
    case SurfaceFormat::Depth16:
        return VK_FORMAT_D16_UNORM;
    case SurfaceFormat::Depth24Stencil8:
        return VK_FORMAT_D24_UNORM_S8_UINT;
    case SurfaceFormat::Depth32:
        return VK_FORMAT_D32_SFLOAT;
    case SurfaceFormat::Depth32_Float_Stencil8_Uint:
        return VK_FORMAT_D32_SFLOAT_S8_UINT;
    }
    return VK_FORMAT_UNDEFINED;
}

} // namespace pomdog::detail::vulkan
