// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vulkan/vulkan_format_helper.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::vulkan {

[[nodiscard]] VkFormat
toSurfaceFormat(PixelFormat format) noexcept
{
    switch (format) {
    case PixelFormat::Invalid:
        return VK_FORMAT_UNDEFINED;
    case PixelFormat::A8_UNorm:
        return VK_FORMAT_R8_UNORM;
    case PixelFormat::B8G8R8A8_UNorm:
        return VK_FORMAT_B8G8R8A8_UNORM;
    case PixelFormat::BlockComp1_UNorm:
        return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
    case PixelFormat::BlockComp2_UNorm:
        return VK_FORMAT_BC2_UNORM_BLOCK;
    case PixelFormat::BlockComp3_UNorm:
        return VK_FORMAT_BC3_UNORM_BLOCK;
    case PixelFormat::R10G10B10A2_UNorm:
        return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
    case PixelFormat::R11G11B10_Float:
        return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
    case PixelFormat::R16G16B16A16_Float:
        return VK_FORMAT_R16G16B16A16_SFLOAT;
    case PixelFormat::R16G16_Float:
        return VK_FORMAT_R16G16_SFLOAT;
    case PixelFormat::R32G32B32A32_Float:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case PixelFormat::R32_Float:
        return VK_FORMAT_R32_SFLOAT;
    case PixelFormat::R8G8B8A8_UNorm:
        return VK_FORMAT_R8G8B8A8_UNORM;
    case PixelFormat::R8G8_UNorm:
        return VK_FORMAT_R8G8_UNORM;
    case PixelFormat::R8_UNorm:
        return VK_FORMAT_R8_UNORM;
    case PixelFormat::Depth16:
        return VK_FORMAT_D16_UNORM;
    case PixelFormat::Depth24Stencil8:
        return VK_FORMAT_D24_UNORM_S8_UINT;
    case PixelFormat::Depth32:
        return VK_FORMAT_D32_SFLOAT;
    case PixelFormat::Depth32_Float_Stencil8_Uint:
        return VK_FORMAT_D32_SFLOAT_S8_UINT;
    }
    return VK_FORMAT_UNDEFINED;
}

[[nodiscard]] VkFormat
chooseDepthStencilFormat(VkPhysicalDevice physicalDevice, PixelFormat format) noexcept
{
    const auto desired = toSurfaceFormat(format);

    // NOTE: Check if the desired format supports depth/stencil attachment
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, desired, &props);
    if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        return desired;
    }

    // NOTE: Build fallback list depending on the requested format
    std::array<VkFormat, 2> fallbacks = {};
    std::size_t fallbackCount = 0;

    switch (format) {
    case PixelFormat::Depth24Stencil8:
        // NOTE: D24S8 is often unsupported on NVIDIA; try D32F_S8
        fallbacks[fallbackCount++] = VK_FORMAT_D32_SFLOAT_S8_UINT;
        break;
    case PixelFormat::Depth16:
        fallbacks[fallbackCount++] = VK_FORMAT_D32_SFLOAT;
        break;
    case PixelFormat::Depth32:
        fallbacks[fallbackCount++] = VK_FORMAT_D32_SFLOAT;
        break;
    case PixelFormat::Depth32_Float_Stencil8_Uint:
        fallbacks[fallbackCount++] = VK_FORMAT_D24_UNORM_S8_UINT;
        break;
    default:
        break;
    }

    for (std::size_t i = 0; i < fallbackCount; ++i) {
        vkGetPhysicalDeviceFormatProperties(physicalDevice, fallbacks[i], &props);
        if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            return fallbacks[i];
        }
    }

    // NOTE: Last resort
    return desired;
}

} // namespace pomdog::gpu::detail::vulkan
