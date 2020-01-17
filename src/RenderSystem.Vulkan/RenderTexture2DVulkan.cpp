// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "RenderTexture2DVulkan.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog::Detail::Vulkan {
namespace {

VkFormat ToDepthStencilFormat(DepthFormat depthFormat) noexcept
{
    POMDOG_ASSERT(depthFormat != DepthFormat::None);
    switch (depthFormat) {
    case DepthFormat::Depth16:
        return VK_FORMAT_D16_UNORM;
    case DepthFormat::Depth24Stencil8:
        return VK_FORMAT_D24_UNORM_S8_UINT;
    case DepthFormat::Depth32:
        return VK_FORMAT_D32_SFLOAT;
    case DepthFormat::Depth32_Float_Stencil8_Uint:
        return VK_FORMAT_D32_SFLOAT_S8_UINT;
    case DepthFormat::None:
        break;
    }
    return VK_FORMAT_D24_UNORM_S8_UINT;
}

} // unnamed namespace

RenderTarget2DVulkan::RenderTarget2DVulkan(
    VkDevice device,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    SurfaceFormat format,
    DepthFormat depthStencilFormat,
    std::int32_t multiSampleCount)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

} // namespace Pomdog::Detail::Vulkan
