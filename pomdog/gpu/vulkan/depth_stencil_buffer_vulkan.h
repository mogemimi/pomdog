// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/depth_stencil_buffer.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/vulkan/prerequisites_vulkan.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::vulkan {

class DepthStencilBufferVulkan final : public DepthStencilBuffer {
private:
    ::VkDevice device_ = nullptr;
    ::VkImage image_ = nullptr;
    ::VkDeviceMemory deviceMemory_ = nullptr;
    ::VkImageView imageView_ = nullptr;
    ::VkFormat vkFormat_ = VK_FORMAT_UNDEFINED;
    i32 pixelWidth_ = 0;
    i32 pixelHeight_ = 0;
    PixelFormat depthStencilFormat_ = PixelFormat::Invalid;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        ::VkDevice device,
        ::VkPhysicalDevice physicalDevice,
        i32 pixelWidth,
        i32 pixelHeight,
        PixelFormat depthStencilFormat,
        i32 multiSampleCount) noexcept;

    ~DepthStencilBufferVulkan() override;

    /// Gets the width of the texture data, in pixels.
    [[nodiscard]] i32
    getWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    [[nodiscard]] i32
    getHeight() const noexcept override;

    /// Gets the format of the pixel data in the depth-stencil buffer.
    [[nodiscard]] PixelFormat
    getFormat() const noexcept override;

    /// Gets the size of the texture resource.
    [[nodiscard]] Rect2D
    getBounds() const noexcept override;

    /// Gets the Vulkan image view for depth/stencil attachment.
    [[nodiscard]] ::VkImageView
    getImageView() const noexcept;

    /// Gets the actual Vulkan format used for the depth/stencil image.
    [[nodiscard]] ::VkFormat
    getVkFormat() const noexcept;
};

} // namespace pomdog::gpu::detail::vulkan
