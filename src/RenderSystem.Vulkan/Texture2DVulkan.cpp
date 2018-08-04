// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Texture2DVulkan.hpp"
#include "VulkanFormatHelper.hpp"
#include "../RenderSystem/TextureHelper.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog {
namespace Detail {
namespace Vulkan {

Texture2DVulkan::Texture2DVulkan(
    ::VkDevice deviceIn,
    ::VkPhysicalDevice physicalDevice,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    SurfaceFormat format)
    : device(deviceIn)
    , image(nullptr)
    , deviceMemory(nullptr)
    , view(nullptr)
{
    POMDOG_ASSERT(device != nullptr);

    const auto pixelFormat = VulkanFormatHelper::ToVkFormat(format);

    if (!pixelFormat) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "This platform does not support this pixel format.");
    }

    VkFormatProperties formatProps;
    vkGetPhysicalDeviceFormatProperties(
        physicalDevice, VK_FORMAT_R8G8B8A8_UNORM, &formatProps);

    const bool canUseLinearTiledImage =
        formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;

    const auto usageFlags = (canUseLinearTiledImage
        ? VK_IMAGE_USAGE_SAMPLED_BIT
        : VK_IMAGE_USAGE_TRANSFER_SRC_BIT);

    VkImageCreateInfo imageCreateInfo;
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.flags = 0;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = *pixelFormat;
    imageCreateInfo.extent.width = pixelWidth;
    imageCreateInfo.extent.height = pixelHeight;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = levelCount;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
    imageCreateInfo.usage = usageFlags;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.queueFamilyIndexCount = 0;
    imageCreateInfo.pQueueFamilyIndices = nullptr;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    // FUS RO DAH!
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");

    ///@todo Not implemented
}

Texture2DVulkan::~Texture2DVulkan()
{
    if (image != nullptr) {
        POMDOG_ASSERT(device != nullptr);
        vkDestroyImage(device, image, nullptr);
    }
    if (deviceMemory != nullptr) {
        POMDOG_ASSERT(device != nullptr);
        vkFreeMemory(device, deviceMemory, nullptr);
    }
    if (view != nullptr) {
        POMDOG_ASSERT(device != nullptr);
        vkDestroyImageView(device, view, nullptr);
    }
}

void Texture2DVulkan::SetData(
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    SurfaceFormat format,
    const void* pixelData)
{
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);
    POMDOG_ASSERT(pixelData != nullptr);
}

} // namespace Vulkan
} // namespace Detail
} // namespace Pomdog
