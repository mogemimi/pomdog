// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/vulkan/prerequisites_vulkan.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::vulkan {

class SwapChainVulkan final {
private:
    ::VkDevice device_ = nullptr;
    ::VkPhysicalDevice physicalDevice_ = nullptr;
    ::VkInstance instance_ = nullptr;
    ::VkSurfaceKHR surface_ = nullptr;
    ::VkSwapchainKHR swapChain_ = nullptr;
    ::VkRenderPass renderPass_ = nullptr;
    ::VkRenderPass loadRenderPass_ = nullptr;

    // Depth buffer resources
    ::VkImage depthImage_ = nullptr;
    ::VkDeviceMemory depthMemory_ = nullptr;
    ::VkImageView depthImageView_ = nullptr;
    ::VkFormat depthFormat_ = VK_FORMAT_UNDEFINED;

    std::vector<::VkImage> images_;
    std::vector<::VkImageView> imageViews_;
    std::vector<::VkFramebuffer> framebuffers_;
    std::vector<::VkSemaphore> imageAvailableSemaphores_;
    std::vector<::VkSemaphore> renderFinishedSemaphores_;
    std::vector<bool> imageInitialized_;

    ::VkFormat surfaceFormat_ = VK_FORMAT_B8G8R8A8_UNORM;
    ::VkExtent2D extent_ = {};
    gpu::PixelFormat depthStencilFormat_ = gpu::PixelFormat::Invalid;
    u32 graphicsQueueFamilyIndex_ = 0;
    u32 currentImageIndex_ = 0;
    u32 frameIndex_ = 0;
    bool imageAcquired_ = false;

public:
    SwapChainVulkan() = default;

    ~SwapChainVulkan();

    /// Initializes the swap chain.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        ::VkInstance instance,
        ::VkPhysicalDevice physicalDevice,
        ::VkDevice device,
        u32 graphicsQueueFamilyIndex,
        ::VkSurfaceKHR surface,
        i32 width,
        i32 height,
        gpu::PixelFormat depthStencilFormat);

    /// Shuts down the swap chain and releases all resources.
    void shutdown();

    /// Acquires the next swap chain image. Idempotent within a frame.
    [[nodiscard]] ::VkResult
    acquireNextImage();

    /// Gets the render pass for the swap chain (clear variant).
    [[nodiscard]] ::VkRenderPass
    getRenderPass() const noexcept;

    /// Gets the clear render pass for the swap chain.
    [[nodiscard]] ::VkRenderPass
    getClearRenderPass() const noexcept;

    /// Gets the load render pass for the swap chain (preserves previous contents).
    [[nodiscard]] ::VkRenderPass
    getLoadRenderPass() const noexcept;

    /// Selects the appropriate render pass for the current image.
    /// Forces CLEAR pass on first use of each image to avoid layout errors.
    [[nodiscard]] ::VkRenderPass
    selectRenderPass(bool hasClearColor) noexcept;

    /// Gets the framebuffer for the currently acquired image.
    [[nodiscard]] ::VkFramebuffer
    getCurrentFramebuffer() const noexcept;

    /// Gets the semaphore signaled when image is available.
    [[nodiscard]] ::VkSemaphore
    getImageAvailableSemaphore() const noexcept;

    /// Gets the semaphore signaled when rendering is finished.
    [[nodiscard]] ::VkSemaphore
    getRenderFinishedSemaphore() const noexcept;

    /// Gets the swap chain handle.
    [[nodiscard]] ::VkSwapchainKHR
    getSwapChain() const noexcept;

    /// Gets the current image index.
    [[nodiscard]] u32
    getCurrentImageIndex() const noexcept;

    /// Gets the swap chain extent.
    [[nodiscard]] ::VkExtent2D
    getExtent() const noexcept;

    /// Gets the surface format.
    [[nodiscard]] ::VkFormat
    getSurfaceFormat() const noexcept;

    /// Returns true if an image has been acquired for this frame.
    [[nodiscard]] bool
    isImageAcquired() const noexcept;

    /// Resets the acquired state after presentation.
    void resetAcquiredState() noexcept;

    /// Recreates the swap chain (e.g., on window resize).
    [[nodiscard]] std::unique_ptr<Error>
    recreate(i32 width, i32 height);

private:
    [[nodiscard]] std::unique_ptr<Error>
    createSwapChain(i32 width, i32 height);

    [[nodiscard]] std::unique_ptr<Error>
    createImageViews();

    [[nodiscard]] std::unique_ptr<Error>
    createRenderPass();

    [[nodiscard]] std::unique_ptr<Error>
    createFramebuffers();

    [[nodiscard]] std::unique_ptr<Error>
    createDepthResources();

    void destroyDepthResources();

    [[nodiscard]] std::unique_ptr<Error>
    createSyncObjects();

    void destroySyncObjects();

    void destroySwapChainResources();
};

} // namespace pomdog::gpu::detail::vulkan
