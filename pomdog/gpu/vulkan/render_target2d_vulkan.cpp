// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vulkan/render_target2d_vulkan.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/vulkan/vulkan_format_helper.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
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

[[nodiscard]] std::tuple<VkRenderPass, std::unique_ptr<Error>>
createRenderPassForRT(
    VkDevice device,
    VkFormat colorFormat,
    VkAttachmentLoadOp loadOp,
    VkImageLayout initialLayout,
    VkFormat depthFormat) noexcept
{
    std::array<VkAttachmentDescription, 2> attachments = {};
    u32 attachmentCount = 0;

    auto& colorAttachment = attachments[attachmentCount];
    colorAttachment.format = colorFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = loadOp;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = initialLayout;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    ++attachmentCount;

    VkAttachmentReference colorRef = {};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthRef = {};
    VkAttachmentReference* pDepthRef = nullptr;

    if (depthFormat != VK_FORMAT_UNDEFINED) {
        auto& depthAttachment = attachments[attachmentCount];
        depthAttachment.format = depthFormat;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        depthRef.attachment = attachmentCount;
        depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        pDepthRef = &depthRef;
        ++attachmentCount;
    }

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;
    subpass.pDepthStencilAttachment = pDepthRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    if (depthFormat != VK_FORMAT_UNDEFINED) {
        dependency.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = attachmentCount;
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkRenderPass renderPass = nullptr;
    auto result = vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
    if (result != VK_SUCCESS) {
        return std::make_tuple(nullptr, errors::make("failed to create VkRenderPass for render target"));
    }
    return std::make_tuple(renderPass, nullptr);
}

} // namespace

std::unique_ptr<Error>
RenderTarget2DVulkan::initialize(
    ::VkDevice device,
    ::VkPhysicalDevice physicalDevice,
    ::VkCommandPool commandPool,
    ::VkQueue graphicsQueue,
    i32 pixelWidth,
    i32 pixelHeight,
    i32 levelCount,
    PixelFormat format,
    [[maybe_unused]] i32 multiSampleCount,
    ::VkFormat depthFormat) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(physicalDevice != nullptr);
    POMDOG_ASSERT(commandPool != nullptr);
    POMDOG_ASSERT(graphicsQueue != nullptr);
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);

    device_ = device;
    physicalDevice_ = physicalDevice;
    commandPool_ = commandPool;
    graphicsQueue_ = graphicsQueue;
    pixelWidth_ = pixelWidth;
    pixelHeight_ = pixelHeight;
    levelCount_ = levelCount;
    format_ = format;
    depthFormat_ = depthFormat;

    const auto vkFormat = toSurfaceFormat(format);

    // NOTE: Create image with color attachment + sampled + transfer source usage
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = vkFormat;
    imageInfo.extent.width = static_cast<u32>(pixelWidth);
    imageInfo.extent.height = static_cast<u32>(pixelHeight);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = static_cast<u32>(levelCount);
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                      VK_IMAGE_USAGE_SAMPLED_BIT |
                      VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    auto result = vkCreateImage(device_, &imageInfo, nullptr, &image_);
    if (result != VK_SUCCESS) {
        return errors::make("failed to create VkImage for render target");
    }

    // NOTE: Allocate device-local memory
    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(device_, image_, &memReqs);

    const auto memTypeIndex = findMemoryType(
        physicalDevice_, memReqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (!memTypeIndex) {
        return errors::make("failed to find suitable memory type for render target");
    }

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = *memTypeIndex;

    result = vkAllocateMemory(device_, &allocInfo, nullptr, &deviceMemory_);
    if (result != VK_SUCCESS) {
        return errors::make("failed to allocate render target memory");
    }

    result = vkBindImageMemory(device_, image_, deviceMemory_, 0);
    if (result != VK_SUCCESS) {
        return errors::make("failed to bind render target memory");
    }

    // NOTE: Create image view
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image_;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = vkFormat;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = static_cast<u32>(levelCount);
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    result = vkCreateImageView(device_, &viewInfo, nullptr, &imageView_);
    if (result != VK_SUCCESS) {
        return errors::make("failed to create VkImageView for render target");
    }

    // NOTE: Create per-render-target depth buffer if the app uses depth
    if (depthFormat_ != VK_FORMAT_UNDEFINED) {
        VkImageCreateInfo depthImageInfo = {};
        depthImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        depthImageInfo.imageType = VK_IMAGE_TYPE_2D;
        depthImageInfo.format = depthFormat_;
        depthImageInfo.extent.width = static_cast<u32>(pixelWidth);
        depthImageInfo.extent.height = static_cast<u32>(pixelHeight);
        depthImageInfo.extent.depth = 1;
        depthImageInfo.mipLevels = 1;
        depthImageInfo.arrayLayers = 1;
        depthImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        depthImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        depthImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        depthImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        depthImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        result = vkCreateImage(device_, &depthImageInfo, nullptr, &depthImage_);
        if (result != VK_SUCCESS) {
            return errors::make("failed to create depth image for render target");
        }

        VkMemoryRequirements depthMemReqs;
        vkGetImageMemoryRequirements(device_, depthImage_, &depthMemReqs);

        const auto depthMemType = findMemoryType(
            physicalDevice_, depthMemReqs.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if (!depthMemType) {
            return errors::make("failed to find memory type for RT depth buffer");
        }

        VkMemoryAllocateInfo depthAllocInfo = {};
        depthAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        depthAllocInfo.allocationSize = depthMemReqs.size;
        depthAllocInfo.memoryTypeIndex = *depthMemType;

        result = vkAllocateMemory(device_, &depthAllocInfo, nullptr, &depthMemory_);
        if (result != VK_SUCCESS) {
            return errors::make("failed to allocate RT depth buffer memory");
        }

        result = vkBindImageMemory(device_, depthImage_, depthMemory_, 0);
        if (result != VK_SUCCESS) {
            return errors::make("failed to bind RT depth buffer memory");
        }

        VkImageAspectFlags depthAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (depthFormat_ == VK_FORMAT_D24_UNORM_S8_UINT ||
            depthFormat_ == VK_FORMAT_D32_SFLOAT_S8_UINT) {
            depthAspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }

        VkImageViewCreateInfo depthViewInfo = {};
        depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        depthViewInfo.image = depthImage_;
        depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        depthViewInfo.format = depthFormat_;
        depthViewInfo.subresourceRange.aspectMask = depthAspect;
        depthViewInfo.subresourceRange.baseMipLevel = 0;
        depthViewInfo.subresourceRange.levelCount = 1;
        depthViewInfo.subresourceRange.baseArrayLayer = 0;
        depthViewInfo.subresourceRange.layerCount = 1;

        result = vkCreateImageView(device_, &depthViewInfo, nullptr, &depthImageView_);
        if (result != VK_SUCCESS) {
            return errors::make("failed to create depth image view for render target");
        }
    }

    // NOTE: Create render pass for clear operations (initialLayout = UNDEFINED, loadOp = CLEAR)
    {
        auto [rp, err] = createRenderPassForRT(
            device_, vkFormat, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_IMAGE_LAYOUT_UNDEFINED, depthFormat_);
        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create clear render pass");
        }
        clearRenderPass_ = rp;
    }

    // NOTE: Create render pass for load operations (initialLayout = SHADER_READ_ONLY, loadOp = LOAD)
    {
        auto [rp, err] = createRenderPassForRT(
            device_, vkFormat, VK_ATTACHMENT_LOAD_OP_LOAD, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, depthFormat_);
        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create load render pass");
        }
        loadRenderPass_ = rp;
    }

    // NOTE: Create framebuffer (compatible with both render passes)
    std::array<VkImageView, 2> fbAttachments = {imageView_, depthImageView_};
    const u32 fbAttachmentCount = (depthImageView_ != nullptr) ? 2 : 1;

    VkFramebufferCreateInfo fbInfo = {};
    fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbInfo.renderPass = clearRenderPass_;
    fbInfo.attachmentCount = fbAttachmentCount;
    fbInfo.pAttachments = fbAttachments.data();
    fbInfo.width = static_cast<u32>(pixelWidth);
    fbInfo.height = static_cast<u32>(pixelHeight);
    fbInfo.layers = 1;

    result = vkCreateFramebuffer(device_, &fbInfo, nullptr, &framebuffer_);
    if (result != VK_SUCCESS) {
        return errors::make("failed to create VkFramebuffer for render target");
    }

    // NOTE: Transition color image from UNDEFINED to SHADER_READ_ONLY_OPTIMAL
    // so it can be safely sampled before the first render pass writes to it.
    {
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

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image_;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = static_cast<u32>(levelCount);
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            cmd,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0, 0, nullptr, 0, nullptr, 1, &barrier);

        vkEndCommandBuffer(cmd);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmd;

        vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue_);

        vkFreeCommandBuffers(device_, commandPool_, 1, &cmd);
    }

    return nullptr;
}

RenderTarget2DVulkan::~RenderTarget2DVulkan()
{
    if (framebuffer_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        vkDestroyFramebuffer(device_, framebuffer_, nullptr);
    }
    if (loadRenderPass_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        vkDestroyRenderPass(device_, loadRenderPass_, nullptr);
    }
    if (clearRenderPass_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        vkDestroyRenderPass(device_, clearRenderPass_, nullptr);
    }
    if (depthImageView_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        vkDestroyImageView(device_, depthImageView_, nullptr);
    }
    if (depthImage_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        vkDestroyImage(device_, depthImage_, nullptr);
    }
    if (depthMemory_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        vkFreeMemory(device_, depthMemory_, nullptr);
    }
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

i32 RenderTarget2DVulkan::getWidth() const noexcept
{
    return pixelWidth_;
}

i32 RenderTarget2DVulkan::getHeight() const noexcept
{
    return pixelHeight_;
}

i32 RenderTarget2DVulkan::getLevelCount() const noexcept
{
    return levelCount_;
}

PixelFormat RenderTarget2DVulkan::getFormat() const noexcept
{
    return format_;
}

Rect2D RenderTarget2DVulkan::getBounds() const noexcept
{
    return Rect2D{0, 0, pixelWidth_, pixelHeight_};
}

void RenderTarget2DVulkan::getData(
    [[maybe_unused]] void* result,
    [[maybe_unused]] std::size_t offsetInBytes,
    [[maybe_unused]] std::size_t sizeInBytes) const
{
    POMDOG_ASSERT(result != nullptr);
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(physicalDevice_ != nullptr);
    POMDOG_ASSERT(image_ != nullptr);

    const auto dataSize = static_cast<VkDeviceSize>(sizeInBytes);

    // Create staging buffer for readback
    VkBuffer stagingBuffer = nullptr;
    VkDeviceMemory stagingMemory = nullptr;

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = dataSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
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

    // NOTE: Transition image to transfer source
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image_;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    vkCmdPipelineBarrier(
        cmd,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0, 0, nullptr, 0, nullptr, 1, &barrier);

    // Copy image to staging buffer
    VkBufferImageCopy region = {};
    region.bufferOffset = static_cast<VkDeviceSize>(offsetInBytes);
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

    vkCmdCopyImageToBuffer(
        cmd, image_,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        stagingBuffer, 1, &region);

    // NOTE: Transition back to shader read
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(
        cmd,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0, 0, nullptr, 0, nullptr, 1, &barrier);

    vkEndCommandBuffer(cmd);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue_);

    // Read back from staging buffer
    void* mapped = nullptr;
    vkMapMemory(device_, stagingMemory, 0, dataSize, 0, &mapped);
    std::memcpy(result, mapped, sizeInBytes);
    vkUnmapMemory(device_, stagingMemory);

    vkFreeCommandBuffers(device_, commandPool_, 1, &cmd);
    vkDestroyBuffer(device_, stagingBuffer, nullptr);
    vkFreeMemory(device_, stagingMemory, nullptr);
}

VkImage RenderTarget2DVulkan::getImage() const noexcept
{
    return image_;
}

VkImageView RenderTarget2DVulkan::getImageView() const noexcept
{
    return imageView_;
}

VkRenderPass RenderTarget2DVulkan::getClearRenderPass() const noexcept
{
    return clearRenderPass_;
}

VkRenderPass RenderTarget2DVulkan::getLoadRenderPass() const noexcept
{
    return loadRenderPass_;
}

VkFramebuffer RenderTarget2DVulkan::getFramebuffer() const noexcept
{
    return framebuffer_;
}

} // namespace pomdog::gpu::detail::vulkan
