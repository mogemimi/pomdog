// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vulkan/texture2d_vulkan.h"
#include "pomdog/gpu/backends/surface_format_helper.h"
#include "pomdog/gpu/backends/texture_helper.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/vulkan/vulkan_format_helper.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cstring>
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

void recordImageLayoutTransition(
    VkCommandBuffer cmd,
    VkImage image,
    VkImageLayout oldLayout,
    VkImageLayout newLayout,
    u32 levelCount) noexcept
{
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = levelCount;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }

    vkCmdPipelineBarrier(
        cmd, srcStage, dstStage, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);
}

} // namespace

std::unique_ptr<Error>
Texture2DVulkan::initialize(
    ::VkDevice deviceIn,
    ::VkPhysicalDevice physicalDevice,
    ::VkCommandPool commandPool,
    ::VkQueue graphicsQueue,
    i32 pixelWidth,
    i32 pixelHeight,
    i32 levelCount,
    PixelFormat format) noexcept
{
    POMDOG_ASSERT(deviceIn != nullptr);
    POMDOG_ASSERT(physicalDevice != nullptr);
    POMDOG_ASSERT(commandPool != nullptr);
    POMDOG_ASSERT(graphicsQueue != nullptr);
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);

    device_ = deviceIn;
    physicalDevice_ = physicalDevice;
    commandPool_ = commandPool;
    graphicsQueue_ = graphicsQueue;
    image_ = nullptr;
    deviceMemory_ = nullptr;
    view_ = nullptr;
    currentLayout_ = VK_IMAGE_LAYOUT_UNDEFINED;
    pixelWidth_ = pixelWidth;
    pixelHeight_ = pixelHeight;
    levelCount_ = levelCount;
    format_ = format;

    const auto vkFormat = toSurfaceFormat(format);

    // NOTE: Create image with optimal tiling for GPU sampling
    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = vkFormat;
    imageCreateInfo.extent.width = static_cast<u32>(pixelWidth);
    imageCreateInfo.extent.height = static_cast<u32>(pixelHeight);
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = static_cast<u32>(levelCount);
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    auto result = vkCreateImage(device_, &imageCreateInfo, nullptr, &image_);
    if (result != VK_SUCCESS) {
        return errors::make("failed to create VkImage");
    }

    // NOTE: Allocate device-local memory
    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(device_, image_, &memReqs);

    const auto memTypeIndex = findMemoryType(
        physicalDevice_, memReqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (!memTypeIndex) {
        return errors::make("failed to find suitable memory type for texture");
    }

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = *memTypeIndex;

    result = vkAllocateMemory(device_, &allocInfo, nullptr, &deviceMemory_);
    if (result != VK_SUCCESS) {
        return errors::make("failed to allocate image memory");
    }

    result = vkBindImageMemory(device_, image_, deviceMemory_, 0);
    if (result != VK_SUCCESS) {
        return errors::make("failed to bind image memory");
    }

    // NOTE: Create image view for sampling
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image_;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = vkFormat;

    // NOTE: For A8_UNorm textures, route the R channel data into the alpha channel
    // so that shaders see (0, 0, 0, glyph_alpha) instead of (glyph, 0, 0, 1).
    if (format == PixelFormat::A8_UNorm) {
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_ZERO;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_ZERO;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_ZERO;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_R;
    }

    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = static_cast<u32>(levelCount);
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    result = vkCreateImageView(device_, &viewInfo, nullptr, &view_);
    if (result != VK_SUCCESS) {
        return errors::make("failed to create VkImageView");
    }

    return nullptr;
}

Texture2DVulkan::~Texture2DVulkan()
{
    if (view_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        vkDestroyImageView(device_, view_, nullptr);
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

void Texture2DVulkan::setData(const void* pixelData)
{
    POMDOG_ASSERT(pixelData != nullptr);
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(physicalDevice_ != nullptr);
    POMDOG_ASSERT(image_ != nullptr);

    // NOTE: Compute data size for mip level 0 only
    const auto dataSize = static_cast<VkDeviceSize>(
        TextureHelper::computeTextureSizeInBytes(pixelWidth_, pixelHeight_, 1, format_));

    // NOTE: Create staging buffer
    VkBuffer stagingBuffer = nullptr;
    VkDeviceMemory stagingMemory = nullptr;

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = dataSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkCreateBuffer(device_, &bufferInfo, nullptr, &stagingBuffer);

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(device_, stagingBuffer, &memReqs);

    const auto memType = findMemoryType(
        physicalDevice_, memReqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = memType.value_or(0);

    vkAllocateMemory(device_, &allocInfo, nullptr, &stagingMemory);
    vkBindBufferMemory(device_, stagingBuffer, stagingMemory, 0);

    // NOTE: Copy pixel data to staging buffer
    void* mapped = nullptr;
    vkMapMemory(device_, stagingMemory, 0, dataSize, 0, &mapped);
    std::memcpy(mapped, pixelData, static_cast<std::size_t>(dataSize));
    vkUnmapMemory(device_, stagingMemory);

    // NOTE: Record and submit transfer commands
    VkCommandBufferAllocateInfo cmdAllocInfo = {};
    cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdAllocInfo.commandPool = commandPool_;
    cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdAllocInfo.commandBufferCount = 1;

    VkCommandBuffer cmd = nullptr;
    vkAllocateCommandBuffers(device_, &cmdAllocInfo, &cmd);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmd, &beginInfo);

    // NOTE: Transition image to transfer destination layout (discard previous contents for full upload)
    recordImageLayoutTransition(
        cmd, image_,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        static_cast<u32>(levelCount_));

    // NOTE: Copy buffer to image (mip level 0)
    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        static_cast<u32>(pixelWidth_),
        static_cast<u32>(pixelHeight_),
        1};

    vkCmdCopyBufferToImage(
        cmd, stagingBuffer, image_,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    // NOTE: Transition to shader read layout
    recordImageLayoutTransition(
        cmd, image_,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        static_cast<u32>(levelCount_));

    vkEndCommandBuffer(cmd);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue_);

    vkFreeCommandBuffers(device_, commandPool_, 1, &cmd);
    vkDestroyBuffer(device_, stagingBuffer, nullptr);
    vkFreeMemory(device_, stagingMemory, nullptr);

    currentLayout_ = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

void Texture2DVulkan::setData(
    i32 mipLevel,
    const Rect2D& region,
    const void* pixelData,
    u32 bytesPerRow)
{
    POMDOG_ASSERT(pixelData != nullptr);
    POMDOG_ASSERT(mipLevel >= 0);
    POMDOG_ASSERT(mipLevel < levelCount_);
    POMDOG_ASSERT(region.width > 0);
    POMDOG_ASSERT(region.height > 0);
    POMDOG_ASSERT(bytesPerRow > 0);
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(physicalDevice_ != nullptr);
    POMDOG_ASSERT(image_ != nullptr);

    const auto dataSize = static_cast<VkDeviceSize>(bytesPerRow) * region.height;

    // NOTE: Create staging buffer
    VkBuffer stagingBuffer = nullptr;
    VkDeviceMemory stagingMemory = nullptr;

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = dataSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkCreateBuffer(device_, &bufferInfo, nullptr, &stagingBuffer);

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(device_, stagingBuffer, &memReqs);

    const auto memType = findMemoryType(
        physicalDevice_, memReqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = memType.value_or(0);

    vkAllocateMemory(device_, &allocInfo, nullptr, &stagingMemory);
    vkBindBufferMemory(device_, stagingBuffer, stagingMemory, 0);

    // NOTE: Copy pixel data to staging buffer
    void* mapped = nullptr;
    vkMapMemory(device_, stagingMemory, 0, dataSize, 0, &mapped);
    std::memcpy(mapped, pixelData, static_cast<std::size_t>(dataSize));
    vkUnmapMemory(device_, stagingMemory);

    // NOTE: Record and submit transfer commands
    VkCommandBufferAllocateInfo cmdAllocInfo = {};
    cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdAllocInfo.commandPool = commandPool_;
    cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdAllocInfo.commandBufferCount = 1;

    VkCommandBuffer cmd = nullptr;
    vkAllocateCommandBuffers(device_, &cmdAllocInfo, &cmd);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmd, &beginInfo);

    // NOTE: Transition image to transfer destination layout
    // Use tracked layout to preserve contents outside the region
    recordImageLayoutTransition(
        cmd, image_,
        currentLayout_,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        static_cast<u32>(levelCount_));

    // NOTE: Copy buffer to image region
    const auto bytesPerPixel = SurfaceFormatHelper::toBytesPerBlock(format_);

    VkBufferImageCopy copyRegion = {};
    copyRegion.bufferOffset = 0;
    copyRegion.bufferRowLength = (bytesPerRow != static_cast<u32>(region.width) * bytesPerPixel)
                                     ? (bytesPerRow / bytesPerPixel)
                                     : 0;
    copyRegion.bufferImageHeight = 0;
    copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyRegion.imageSubresource.mipLevel = static_cast<u32>(mipLevel);
    copyRegion.imageSubresource.baseArrayLayer = 0;
    copyRegion.imageSubresource.layerCount = 1;
    copyRegion.imageOffset = {region.x, region.y, 0};
    copyRegion.imageExtent = {
        static_cast<u32>(region.width),
        static_cast<u32>(region.height),
        1};

    vkCmdCopyBufferToImage(
        cmd, stagingBuffer, image_,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

    // NOTE: Transition to shader read layout
    recordImageLayoutTransition(
        cmd, image_,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        static_cast<u32>(levelCount_));

    vkEndCommandBuffer(cmd);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue_);

    vkFreeCommandBuffers(device_, commandPool_, 1, &cmd);
    vkDestroyBuffer(device_, stagingBuffer, nullptr);
    vkFreeMemory(device_, stagingMemory, nullptr);

    currentLayout_ = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

i32 Texture2DVulkan::getWidth() const noexcept
{
    return pixelWidth_;
}

i32 Texture2DVulkan::getHeight() const noexcept
{
    return pixelHeight_;
}

i32 Texture2DVulkan::getLevelCount() const noexcept
{
    return levelCount_;
}

PixelFormat Texture2DVulkan::getFormat() const noexcept
{
    return format_;
}

VkImageView Texture2DVulkan::getImageView() const noexcept
{
    return view_;
}

} // namespace pomdog::gpu::detail::vulkan
