// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/render_target2d.h"
#include "pomdog/gpu/vulkan/prerequisites_vulkan.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::vulkan {

class RenderTarget2DVulkan final : public RenderTarget2D {
private:
    ::VkDevice device_ = nullptr;
    ::VkPhysicalDevice physicalDevice_ = nullptr;
    ::VkCommandPool commandPool_ = nullptr;
    ::VkQueue graphicsQueue_ = nullptr;
    ::VkImage image_ = nullptr;
    ::VkDeviceMemory deviceMemory_ = nullptr;
    ::VkImageView imageView_ = nullptr;
    ::VkImage depthImage_ = nullptr;
    ::VkDeviceMemory depthMemory_ = nullptr;
    ::VkImageView depthImageView_ = nullptr;
    ::VkRenderPass clearRenderPass_ = nullptr;
    ::VkRenderPass loadRenderPass_ = nullptr;
    ::VkFramebuffer framebuffer_ = nullptr;
    ::VkFormat depthFormat_ = VK_FORMAT_UNDEFINED;
    i32 pixelWidth_ = 0;
    i32 pixelHeight_ = 0;
    i32 levelCount_ = 0;
    PixelFormat format_ = {};

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        ::VkDevice device,
        ::VkPhysicalDevice physicalDevice,
        ::VkCommandPool commandPool,
        ::VkQueue graphicsQueue,
        i32 pixelWidth,
        i32 pixelHeight,
        i32 levelCount,
        PixelFormat format,
        i32 multiSampleCount,
        ::VkFormat depthFormat) noexcept;

    ~RenderTarget2DVulkan() override;

    /// Gets the width of the texture data, in pixels.
    [[nodiscard]] i32
    getWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    [[nodiscard]] i32
    getHeight() const noexcept override;

    /// Gets the mipmap level.
    [[nodiscard]] i32
    getLevelCount() const noexcept override;

    /// Gets the format of the pixel data in the render target.
    [[nodiscard]] PixelFormat
    getFormat() const noexcept override;

    /// Gets the size of the texture resource.
    [[nodiscard]] Rect2D
    getBounds() const noexcept override;

    /// Copies the pixel data from texture to memory.
    void getData(void* result, std::size_t offsetInBytes, std::size_t sizeInBytes) const override;

    /// Gets the Vulkan image.
    [[nodiscard]] ::VkImage
    getImage() const noexcept;

    /// Gets the Vulkan image view.
    [[nodiscard]] ::VkImageView
    getImageView() const noexcept;

    /// Gets the render pass for clear operations.
    [[nodiscard]] ::VkRenderPass
    getClearRenderPass() const noexcept;

    /// Gets the render pass for load operations.
    [[nodiscard]] ::VkRenderPass
    getLoadRenderPass() const noexcept;

    /// Gets the framebuffer.
    [[nodiscard]] ::VkFramebuffer
    getFramebuffer() const noexcept;
};

} // namespace pomdog::gpu::detail::vulkan
