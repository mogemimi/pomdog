// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/graphics/vulkan/texture2d_vulkan.hpp"
#include "pomdog/graphics/backends/texture_helper.hpp"
#include "pomdog/graphics/surface_format.hpp"
#include "pomdog/graphics/vulkan/buffer_vulkan.hpp"
#include "pomdog/graphics/vulkan/vulkan_format_helper.hpp"
#include "pomdog/utility/assert.hpp"
#include "pomdog/utility/exception.hpp"

namespace Pomdog::Detail::Vulkan {

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

    const auto pixelFormat = ToSurfaceFormat(format);

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
    imageCreateInfo.format = pixelFormat;
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

    VkImage mappableImage = nullptr;
    VkDeviceMemory mappableMemory = nullptr;
    VkMemoryRequirements memoryRequirements;

    VkResult result = vkCreateImage(device, &imageCreateInfo, nullptr, &mappableImage);
    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create VkImage");
    }

    vkGetImageMemoryRequirements(device, mappableImage, &memoryRequirements);

    VkMemoryAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = 0;

    result = vkAllocateMemory(device, &allocInfo, nullptr, &mappableMemory);
    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to call vkAllocateMemory()");
    }

    result = vkBindImageMemory(device, mappableImage, mappableMemory, 0);
    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to call vkBindImageMemory()");
    }

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

void Texture2DVulkan::SetData(const void* pixelData)
{
    POMDOG_ASSERT(pixelData != nullptr);
}

} // namespace Pomdog::Detail::Vulkan
