// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "VulkanFormatHelper.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace Vulkan {

std::optional<VkFormat> VulkanFormatHelper::ToVkFormat(SurfaceFormat format) noexcept
{
    switch (format) {
    case SurfaceFormat::A8_UNorm: return VK_FORMAT_R8_UNORM;
    case SurfaceFormat::B8G8R8A8_UNorm: return VK_FORMAT_B8G8R8A8_UNORM;
    case SurfaceFormat::BlockComp1_UNorm: return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
    case SurfaceFormat::BlockComp2_UNorm: return VK_FORMAT_BC2_UNORM_BLOCK;
    case SurfaceFormat::BlockComp3_UNorm: return VK_FORMAT_BC3_UNORM_BLOCK;
    case SurfaceFormat::R10G10B10A2_UNorm: return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
    case SurfaceFormat::R11G11B10_Float: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
    case SurfaceFormat::R16G16B16A16_Float: return VK_FORMAT_R16G16B16A16_SFLOAT;
    case SurfaceFormat::R16G16_Float: return VK_FORMAT_R16G16_SFLOAT;
    case SurfaceFormat::R32G32B32A32_Float: return VK_FORMAT_R32G32B32A32_SFLOAT;
    case SurfaceFormat::R32_Float: return VK_FORMAT_R32_SFLOAT;
    case SurfaceFormat::R8G8B8A8_UNorm: return VK_FORMAT_R8G8B8A8_UNORM;
    case SurfaceFormat::R8G8_UNorm: return VK_FORMAT_R8G8_UNORM;
    case SurfaceFormat::R8_UNorm: return VK_FORMAT_R8_UNORM;
    }
    return std::nullopt;
}

} // namespace Vulkan
} // namespace Detail
} // namespace Pomdog
