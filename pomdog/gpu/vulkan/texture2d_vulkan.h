// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/gpu/vulkan/prerequisites_vulkan.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::vulkan {

/// Texture2DVulkan is the Vulkan implementation of Texture2D.
class Texture2DVulkan final : public Texture2D {
private:
    ::VkDevice device_ = nullptr;
    ::VkPhysicalDevice physicalDevice_ = nullptr;
    ::VkCommandPool commandPool_ = nullptr;
    ::VkQueue graphicsQueue_ = nullptr;
    ::VkImage image_ = nullptr;
    ::VkDeviceMemory deviceMemory_ = nullptr;
    ::VkImageView view_ = nullptr;
    ::VkImageLayout currentLayout_ = VK_IMAGE_LAYOUT_UNDEFINED;
    i32 pixelWidth_ = 0;
    i32 pixelHeight_ = 0;
    i32 levelCount_ = 0;
    PixelFormat format_ = {};

public:
    /// Creates a VkImage with device memory and an image view for the given dimensions and format.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        ::VkDevice device,
        ::VkPhysicalDevice physicalDevice,
        ::VkCommandPool commandPool,
        ::VkQueue graphicsQueue,
        i32 pixelWidth,
        i32 pixelHeight,
        i32 levelCount,
        PixelFormat format) noexcept;

    ~Texture2DVulkan() override;

    /// Gets the width of the texture data, in pixels.
    [[nodiscard]] i32
    getWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    [[nodiscard]] i32
    getHeight() const noexcept override;

    /// Gets the mipmap level.
    [[nodiscard]] i32
    getLevelCount() const noexcept override;

    /// Gets the format of the pixel data in the texture.
    [[nodiscard]] PixelFormat
    getFormat() const noexcept override;

    /// Sets texture data.
    void setData(const void* pixelData) override;

    /// Sets texture data for a sub-region.
    void setData(
        i32 mipLevel,
        const Rect2D& region,
        const void* pixelData,
        u32 bytesPerRow) override;

    /// Gets the Vulkan image view for shader binding.
    [[nodiscard]] ::VkImageView
    getImageView() const noexcept;
};

} // namespace pomdog::gpu::detail::vulkan
