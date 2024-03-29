// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/texture2d.h"
#include <vulkan/vulkan.h>

namespace pomdog::gpu::detail::vulkan {

class Texture2DVulkan final : public Texture2D {
public:
    Texture2DVulkan(
        ::VkDevice device,
        ::VkPhysicalDevice physicalDevice,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        PixelFormat format);

    ~Texture2DVulkan() override;

    /// Gets the width of the texture data, in pixels.
    std::int32_t GetWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    std::int32_t GetHeight() const noexcept override;

    /// Gets the mipmap level.
    std::int32_t GetLevelCount() const noexcept override;

    /// Gets the format of the pixel data in the texture.
    PixelFormat GetFormat() const noexcept override;

    /// Sets texture data.
    void SetData(const void* pixelData) override;

private:
    ::VkDevice device;
    ::VkImage image;
    ::VkDeviceMemory deviceMemory;
    ::VkImageView view;
};

} // namespace pomdog::gpu::detail::vulkan
