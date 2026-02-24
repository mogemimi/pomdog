// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/texture2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vulkan/vulkan.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::vulkan {

class Texture2DVulkan final : public Texture2D {
public:
    Texture2DVulkan(
        ::VkDevice device,
        ::VkPhysicalDevice physicalDevice,
        i32 pixelWidth,
        i32 pixelHeight,
        i32 levelCount,
        PixelFormat format);

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

private:
    ::VkDevice device;
    ::VkImage image;
    ::VkDeviceMemory deviceMemory;
    ::VkImageView view;
};

} // namespace pomdog::gpu::detail::vulkan
