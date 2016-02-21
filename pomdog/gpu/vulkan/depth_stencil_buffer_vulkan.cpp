// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vulkan/depth_stencil_buffer_vulkan.h"
#include "pomdog/gpu/vulkan/vulkan_format_helper.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::vulkan {
namespace {

[[nodiscard]] std::optional<u32>
findMemoryType(VkPhysicalDevice physicalDevice, u32 typeFilter, VkMemoryPropertyFlags propertyFlags) noexcept
{
    VkPhysicalDeviceMemoryProperties props;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &props);
    for (u32 i = 0; i < props.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            ((props.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags)) {
            return i;
        }
    }
    return std::nullopt;
}

[[nodiscard]] VkImageAspectFlags
toDepthStencilAspect(PixelFormat format) noexcept
{
    switch (format) {
    case PixelFormat::Depth16:
    case PixelFormat::Depth32:
        return VK_IMAGE_ASPECT_DEPTH_BIT;
    case PixelFormat::Depth24Stencil8:
    case PixelFormat::Depth32_Float_Stencil8_Uint:
        return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    default:
        return VK_IMAGE_ASPECT_DEPTH_BIT;
    }
}

} // namespace

std::unique_ptr<Error>
DepthStencilBufferVulkan::initialize(
    ::VkDevice deviceIn,
    ::VkPhysicalDevice physicalDevice,
    i32 pixelWidth,
    i32 pixelHeight,
    PixelFormat depthStencilFormat,
    [[maybe_unused]] i32 multiSampleCount) noexcept
{
    POMDOG_ASSERT(deviceIn != nullptr);
    POMDOG_ASSERT(physicalDevice != nullptr);
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);

    if (depthStencilFormat == PixelFormat::Invalid) {
        return errors::make("depthStencilFormat must be != PixelFormat::Invalid");
    }

    device_ = deviceIn;
    pixelWidth_ = pixelWidth;
    pixelHeight_ = pixelHeight;
    depthStencilFormat_ = depthStencilFormat;

    const auto vkFormat = chooseDepthStencilFormat(physicalDevice, depthStencilFormat);
    vkFormat_ = vkFormat;

    // Create depth/stencil image
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = vkFormat;
    imageInfo.extent.width = static_cast<u32>(pixelWidth);
    imageInfo.extent.height = static_cast<u32>(pixelHeight);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    auto result = vkCreateImage(device_, &imageInfo, nullptr, &image_);
    if (result != VK_SUCCESS) {
        return errors::make("failed to create VkImage for depth stencil buffer");
    }

    // Allocate device-local memory
    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(device_, image_, &memReqs);

    const auto memTypeIndex = findMemoryType(
        physicalDevice, memReqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (!memTypeIndex) {
        return errors::make("failed to find suitable memory type for depth stencil buffer");
    }

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = *memTypeIndex;

    result = vkAllocateMemory(device_, &allocInfo, nullptr, &deviceMemory_);
    if (result != VK_SUCCESS) {
        return errors::make("failed to allocate depth stencil buffer memory");
    }

    result = vkBindImageMemory(device_, image_, deviceMemory_, 0);
    if (result != VK_SUCCESS) {
        return errors::make("failed to bind depth stencil buffer memory");
    }

    // Create image view
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image_;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = vkFormat;
    viewInfo.subresourceRange.aspectMask = toDepthStencilAspect(depthStencilFormat);
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    result = vkCreateImageView(device_, &viewInfo, nullptr, &imageView_);
    if (result != VK_SUCCESS) {
        return errors::make("failed to create VkImageView for depth stencil buffer");
    }

    return nullptr;
}

DepthStencilBufferVulkan::~DepthStencilBufferVulkan()
{
    if (imageView_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        vkDestroyImageView(device_, imageView_, nullptr);
    }
    if (image_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        vkDestroyImage(device_, image_, nullptr);
    }
    if (deviceMemory_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        vkFreeMemory(device_, deviceMemory_, nullptr);
    }
}

i32 DepthStencilBufferVulkan::getWidth() const noexcept
{
    return pixelWidth_;
}

i32 DepthStencilBufferVulkan::getHeight() const noexcept
{
    return pixelHeight_;
}

PixelFormat DepthStencilBufferVulkan::getFormat() const noexcept
{
    return depthStencilFormat_;
}

Rect2D DepthStencilBufferVulkan::getBounds() const noexcept
{
    return Rect2D{0, 0, pixelWidth_, pixelHeight_};
}

VkImageView DepthStencilBufferVulkan::getImageView() const noexcept
{
    return imageView_;
}

VkFormat DepthStencilBufferVulkan::getVkFormat() const noexcept
{
    return vkFormat_;
}

} // namespace pomdog::gpu::detail::vulkan
