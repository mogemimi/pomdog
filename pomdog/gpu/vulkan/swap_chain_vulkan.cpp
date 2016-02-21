// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vulkan/swap_chain_vulkan.h"
#include "pomdog/gpu/vulkan/vulkan_format_helper.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <limits>
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

} // namespace

SwapChainVulkan::~SwapChainVulkan()
{
    shutdown();
}

std::unique_ptr<Error>
SwapChainVulkan::initialize(
    ::VkInstance instance,
    ::VkPhysicalDevice physicalDevice,
    ::VkDevice device,
    u32 graphicsQueueFamilyIndex,
    ::VkSurfaceKHR surface,
    i32 width,
    i32 height,
    gpu::PixelFormat depthStencilFormat)
{
    POMDOG_ASSERT(instance != nullptr);
    POMDOG_ASSERT(physicalDevice != nullptr);
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(surface != nullptr);

    instance_ = instance;
    physicalDevice_ = physicalDevice;
    device_ = device;
    graphicsQueueFamilyIndex_ = graphicsQueueFamilyIndex;
    surface_ = surface;
    depthStencilFormat_ = depthStencilFormat;

    // NOTE: Resolve the depth format via physical device query
    if (depthStencilFormat_ != gpu::PixelFormat::Invalid) {
        depthFormat_ = chooseDepthStencilFormat(physicalDevice_, depthStencilFormat_);
    }

    // NOTE: Verify that the graphics queue supports presentation
    VkBool32 presentSupport = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(
        physicalDevice_, graphicsQueueFamilyIndex_, surface_, &presentSupport);
    if (presentSupport != VK_TRUE) {
        return errors::make("graphics queue does not support presentation to this surface");
    }

    // NOTE: Create swap chain (determines surface format and extent)
    if (auto err = createSwapChain(width, height); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create swap chain");
    }

    // NOTE: Create render pass (uses surface format from swap chain)
    if (auto err = createRenderPass(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create render pass");
    }

    // NOTE: Create depth buffer if requested
    if (depthFormat_ != VK_FORMAT_UNDEFINED) {
        if (auto err = createDepthResources(); err != nullptr) {
            return errors::wrap(std::move(err), "failed to create depth resources");
        }
    }

    // NOTE: Create image views for swap chain images
    if (auto err = createImageViews(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create image views");
    }

    // NOTE: Create framebuffers using image views and render pass
    if (auto err = createFramebuffers(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create framebuffers");
    }

    // NOTE: Create per-image synchronization semaphores
    if (auto err = createSyncObjects(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create sync objects");
    }

    return nullptr;
}

void SwapChainVulkan::shutdown()
{
    if (device_ == nullptr) {
        return;
    }

    vkDeviceWaitIdle(device_);

    destroySwapChainResources();
    destroySyncObjects();
    destroyDepthResources();

    if (renderPass_ != nullptr) {
        vkDestroyRenderPass(device_, renderPass_, nullptr);
        renderPass_ = nullptr;
    }

    if (loadRenderPass_ != nullptr) {
        vkDestroyRenderPass(device_, loadRenderPass_, nullptr);
        loadRenderPass_ = nullptr;
    }

    if (surface_ != nullptr && instance_ != nullptr) {
        vkDestroySurfaceKHR(instance_, surface_, nullptr);
        surface_ = nullptr;
    }

    device_ = nullptr;
}

VkResult SwapChainVulkan::acquireNextImage()
{
    if (imageAcquired_) {
        return VK_SUCCESS;
    }

    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(swapChain_ != nullptr);
    POMDOG_ASSERT(frameIndex_ < imageAvailableSemaphores_.size());

    auto result = vkAcquireNextImageKHR(
        device_,
        swapChain_,
        std::numeric_limits<u64>::max(),
        imageAvailableSemaphores_[frameIndex_],
        nullptr, // fence
        &currentImageIndex_);

    if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR) {
        imageAcquired_ = true;
    }

    return result;
}

VkRenderPass SwapChainVulkan::getRenderPass() const noexcept
{
    return renderPass_;
}

VkRenderPass SwapChainVulkan::getClearRenderPass() const noexcept
{
    return renderPass_;
}

VkRenderPass SwapChainVulkan::getLoadRenderPass() const noexcept
{
    return loadRenderPass_;
}

VkRenderPass SwapChainVulkan::selectRenderPass(bool hasClearColor) noexcept
{
    POMDOG_ASSERT(currentImageIndex_ < imageInitialized_.size());
    if (hasClearColor || !imageInitialized_[currentImageIndex_]) {
        imageInitialized_[currentImageIndex_] = true;
        return renderPass_; // CLEAR pass (initialLayout = UNDEFINED, always safe)
    }
    return loadRenderPass_; // LOAD pass (initialLayout = PRESENT_SRC_KHR)
}

VkFramebuffer SwapChainVulkan::getCurrentFramebuffer() const noexcept
{
    POMDOG_ASSERT(imageAcquired_);
    POMDOG_ASSERT(currentImageIndex_ < framebuffers_.size());
    return framebuffers_[currentImageIndex_];
}

VkSemaphore SwapChainVulkan::getImageAvailableSemaphore() const noexcept
{
    POMDOG_ASSERT(frameIndex_ < imageAvailableSemaphores_.size());
    return imageAvailableSemaphores_[frameIndex_];
}

VkSemaphore SwapChainVulkan::getRenderFinishedSemaphore() const noexcept
{
    POMDOG_ASSERT(frameIndex_ < renderFinishedSemaphores_.size());
    return renderFinishedSemaphores_[frameIndex_];
}

VkSwapchainKHR SwapChainVulkan::getSwapChain() const noexcept
{
    return swapChain_;
}

u32 SwapChainVulkan::getCurrentImageIndex() const noexcept
{
    return currentImageIndex_;
}

VkExtent2D SwapChainVulkan::getExtent() const noexcept
{
    return extent_;
}

VkFormat SwapChainVulkan::getSurfaceFormat() const noexcept
{
    return surfaceFormat_;
}

bool SwapChainVulkan::isImageAcquired() const noexcept
{
    return imageAcquired_;
}

void SwapChainVulkan::resetAcquiredState() noexcept
{
    imageAcquired_ = false;
    // NOTE: Advance to the next frame's semaphore pair
    if (!imageAvailableSemaphores_.empty()) {
        frameIndex_ = (frameIndex_ + 1) % static_cast<u32>(imageAvailableSemaphores_.size());
    }
}

std::unique_ptr<Error>
SwapChainVulkan::recreate(i32 width, i32 height)
{
    POMDOG_ASSERT(device_ != nullptr);
    vkDeviceWaitIdle(device_);

    destroySwapChainResources();

    if (auto err = createSwapChain(width, height); err != nullptr) {
        return errors::wrap(std::move(err), "failed to recreate swap chain");
    }

    // NOTE: Render pass format may have changed, recreate
    if (renderPass_ != nullptr) {
        vkDestroyRenderPass(device_, renderPass_, nullptr);
        renderPass_ = nullptr;
    }
    if (loadRenderPass_ != nullptr) {
        vkDestroyRenderPass(device_, loadRenderPass_, nullptr);
        loadRenderPass_ = nullptr;
    }
    if (auto err = createRenderPass(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to recreate render pass");
    }

    // NOTE: Recreate depth buffer for new extent
    destroyDepthResources();
    if (depthFormat_ != VK_FORMAT_UNDEFINED) {
        if (auto err = createDepthResources(); err != nullptr) {
            return errors::wrap(std::move(err), "failed to recreate depth resources");
        }
    }

    if (auto err = createImageViews(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to recreate image views");
    }

    if (auto err = createFramebuffers(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to recreate framebuffers");
    }

    // NOTE: Recreate semaphores for the new image count
    destroySyncObjects();
    if (auto err = createSyncObjects(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to recreate sync objects");
    }

    imageAcquired_ = false;
    frameIndex_ = 0;
    return nullptr;
}

std::unique_ptr<Error>
SwapChainVulkan::createSwapChain(i32 width, i32 height)
{
    POMDOG_ASSERT(physicalDevice_ != nullptr);
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(surface_ != nullptr);

    // NOTE: Query surface capabilities
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice_, surface_, &capabilities);

    // NOTE: Query supported surface formats
    u32 formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice_, surface_, &formatCount, nullptr);
    if (formatCount == 0) {
        return errors::make("no surface formats available");
    }
    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice_, surface_, &formatCount, surfaceFormats.data());

    // NOTE: Choose the best surface format (prefer B8G8R8A8_UNORM + sRGB)
    VkSurfaceFormatKHR chosenFormat = surfaceFormats[0];
    for (const auto& format : surfaceFormats) {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            chosenFormat = format;
            break;
        }
    }
    surfaceFormat_ = chosenFormat.format;

    // NOTE: Query supported present modes
    u32 presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice_, surface_, &presentModeCount, nullptr);
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    if (presentModeCount > 0) {
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            physicalDevice_, surface_, &presentModeCount, presentModes.data());
    }

    // NOTE: Choose present mode (prefer MAILBOX for low-latency, fallback FIFO)
    VkPresentModeKHR chosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (const auto& mode : presentModes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            chosenPresentMode = mode;
            break;
        }
    }

    // NOTE: Choose swap extent
    if (capabilities.currentExtent.width != std::numeric_limits<u32>::max()) {
        extent_ = capabilities.currentExtent;
    }
    else {
        extent_.width = std::clamp(
            static_cast<u32>(width),
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width);
        extent_.height = std::clamp(
            static_cast<u32>(height),
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height);
    }

    // NOTE: Choose image count (one more than minimum for triple buffering)
    u32 imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }

    // NOTE: Create the swap chain
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface_;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat_;
    createInfo.imageColorSpace = chosenFormat.colorSpace;
    createInfo.imageExtent = extent_;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = chosenPresentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = nullptr;

    auto result = vkCreateSwapchainKHR(device_, &createInfo, nullptr, &swapChain_);
    if (result != VK_SUCCESS) {
        return errors::make("failed to create Vulkan swap chain");
    }

    // NOTE: Get the swap chain images
    vkGetSwapchainImagesKHR(device_, swapChain_, &imageCount, nullptr);
    images_.resize(imageCount);
    vkGetSwapchainImagesKHR(device_, swapChain_, &imageCount, images_.data());

    // NOTE: Track per-image initialization state for layout transitions
    imageInitialized_.assign(imageCount, false);

    return nullptr;
}

std::unique_ptr<Error>
SwapChainVulkan::createImageViews()
{
    POMDOG_ASSERT(device_ != nullptr);

    imageViews_.resize(images_.size());

    for (std::size_t i = 0; i < images_.size(); i++) {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = images_[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = surfaceFormat_;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device_, &createInfo, nullptr, &imageViews_[i]) != VK_SUCCESS) {
            return errors::make("failed to create swap chain image view");
        }
    }

    return nullptr;
}

std::unique_ptr<Error>
SwapChainVulkan::createRenderPass()
{
    POMDOG_ASSERT(device_ != nullptr);

    // NOTE: Color attachment for the swap chain image
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = surfaceFormat_;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // NOTE: Prepare attachments array: color, and optionally depth
    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, {}};
    u32 attachmentCount = 1;

    VkAttachmentReference depthAttachmentRef = {};
    VkAttachmentReference* pDepthRef = nullptr;

    if (depthFormat_ != VK_FORMAT_UNDEFINED) {
        auto& depthAttachment = attachments[1];
        depthAttachment.format = depthFormat_;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        pDepthRef = &depthAttachmentRef;
        attachmentCount = 2;
    }

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = pDepthRef;

    // NOTE: Subpass dependency to ensure color attachment output is ready
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    if (depthFormat_ != VK_FORMAT_UNDEFINED) {
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

    if (vkCreateRenderPass(device_, &renderPassInfo, nullptr, &renderPass_) != VK_SUCCESS) {
        return errors::make("failed to create render pass");
    }

    // NOTE: Create load render pass (preserves previous contents)
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachments[0] = colorAttachment;

    if (depthFormat_ != VK_FORMAT_UNDEFINED) {
        attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    if (vkCreateRenderPass(device_, &renderPassInfo, nullptr, &loadRenderPass_) != VK_SUCCESS) {
        return errors::make("failed to create load render pass");
    }

    return nullptr;
}

std::unique_ptr<Error>
SwapChainVulkan::createFramebuffers()
{
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(renderPass_ != nullptr);

    framebuffers_.resize(imageViews_.size());

    for (std::size_t i = 0; i < imageViews_.size(); i++) {
        std::array<VkImageView, 2> attachments = {imageViews_[i], depthImageView_};
        const u32 attachmentCount = (depthImageView_ != nullptr) ? 2 : 1;

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass_;
        framebufferInfo.attachmentCount = attachmentCount;
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = extent_.width;
        framebufferInfo.height = extent_.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device_, &framebufferInfo, nullptr, &framebuffers_[i]) != VK_SUCCESS) {
            return errors::make("failed to create framebuffer");
        }
    }

    return nullptr;
}

void SwapChainVulkan::destroySwapChainResources()
{
    if (device_ == nullptr) {
        return;
    }

    for (auto& fb : framebuffers_) {
        if (fb != nullptr) {
            vkDestroyFramebuffer(device_, fb, nullptr);
        }
    }
    framebuffers_.clear();

    for (auto& iv : imageViews_) {
        if (iv != nullptr) {
            vkDestroyImageView(device_, iv, nullptr);
        }
    }
    imageViews_.clear();

    images_.clear();

    if (swapChain_ != nullptr) {
        vkDestroySwapchainKHR(device_, swapChain_, nullptr);
        swapChain_ = nullptr;
    }
}

std::unique_ptr<Error>
SwapChainVulkan::createDepthResources()
{
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(physicalDevice_ != nullptr);
    POMDOG_ASSERT(depthFormat_ != VK_FORMAT_UNDEFINED);

    // Create depth image
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = depthFormat_;
    imageInfo.extent.width = extent_.width;
    imageInfo.extent.height = extent_.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (vkCreateImage(device_, &imageInfo, nullptr, &depthImage_) != VK_SUCCESS) {
        return errors::make("failed to create swap chain depth image");
    }

    // NOTE: Allocate device-local memory
    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(device_, depthImage_, &memReqs);

    const auto memTypeIndex = findMemoryType(
        physicalDevice_, memReqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (!memTypeIndex) {
        return errors::make("failed to find suitable memory type for depth image");
    }

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = *memTypeIndex;

    if (vkAllocateMemory(device_, &allocInfo, nullptr, &depthMemory_) != VK_SUCCESS) {
        return errors::make("failed to allocate depth image memory");
    }

    if (vkBindImageMemory(device_, depthImage_, depthMemory_, 0) != VK_SUCCESS) {
        return errors::make("failed to bind depth image memory");
    }

    // NOTE: Determine aspect mask
    VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    if (depthFormat_ == VK_FORMAT_D24_UNORM_S8_UINT ||
        depthFormat_ == VK_FORMAT_D32_SFLOAT_S8_UINT) {
        aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    // NOTE: Create depth image view
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = depthImage_;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = depthFormat_;
    viewInfo.subresourceRange.aspectMask = aspectMask;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device_, &viewInfo, nullptr, &depthImageView_) != VK_SUCCESS) {
        return errors::make("failed to create depth image view");
    }

    return nullptr;
}

void SwapChainVulkan::destroyDepthResources()
{
    if (device_ == nullptr) {
        return;
    }
    if (depthImageView_ != nullptr) {
        vkDestroyImageView(device_, depthImageView_, nullptr);
        depthImageView_ = nullptr;
    }
    if (depthImage_ != nullptr) {
        vkDestroyImage(device_, depthImage_, nullptr);
        depthImage_ = nullptr;
    }
    if (depthMemory_ != nullptr) {
        vkFreeMemory(device_, depthMemory_, nullptr);
        depthMemory_ = nullptr;
    }
}

std::unique_ptr<Error>
SwapChainVulkan::createSyncObjects()
{
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(!images_.empty());

    const auto count = images_.size();
    imageAvailableSemaphores_.resize(count);
    renderFinishedSemaphores_.resize(count);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (std::size_t i = 0; i < count; i++) {
        if (vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &imageAvailableSemaphores_[i]) != VK_SUCCESS) {
            return errors::make("failed to create imageAvailable semaphore");
        }
        if (vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &renderFinishedSemaphores_[i]) != VK_SUCCESS) {
            return errors::make("failed to create renderFinished semaphore");
        }
    }

    frameIndex_ = 0;
    return nullptr;
}

void SwapChainVulkan::destroySyncObjects()
{
    if (device_ == nullptr) {
        return;
    }

    for (auto& sem : renderFinishedSemaphores_) {
        if (sem != nullptr) {
            vkDestroySemaphore(device_, sem, nullptr);
        }
    }
    renderFinishedSemaphores_.clear();

    for (auto& sem : imageAvailableSemaphores_) {
        if (sem != nullptr) {
            vkDestroySemaphore(device_, sem, nullptr);
        }
    }
    imageAvailableSemaphores_.clear();
}

} // namespace pomdog::gpu::detail::vulkan
