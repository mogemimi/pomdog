// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/texture2d.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog::Detail::Vulkan {

class Texture2DVulkan final : public Texture2D {
public:
    Texture2DVulkan(
        ::VkDevice device,
        ::VkPhysicalDevice physicalDevice,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format);

    ~Texture2DVulkan() override;

    /// Gets the width of the texture data, in pixels.
    std::int32_t GetWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    std::int32_t GetHeight() const noexcept override;

    /// Gets the mipmap level.
    std::int32_t GetLevelCount() const noexcept override;

    /// Gets the format of the pixel data in the texture.
    SurfaceFormat GetFormat() const noexcept override;

    /// Sets texture data.
    void SetData(const void* pixelData) override;

private:
    ::VkDevice device;
    ::VkImage image;
    ::VkDeviceMemory deviceMemory;
    ::VkImageView view;
};

} // namespace Pomdog::Detail::Vulkan
