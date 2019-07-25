// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativeTexture2D.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog {
namespace Detail {
namespace Vulkan {

class Texture2DVulkan final : public NativeTexture2D {
public:
    Texture2DVulkan(
        ::VkDevice device,
        ::VkPhysicalDevice physicalDevice,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format);

    ~Texture2DVulkan();

    void SetData(
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format,
        const void* pixelData) override;

private:
    ::VkDevice device;
    ::VkImage image;
    ::VkDeviceMemory deviceMemory;
    ::VkImageView view;
};

} // namespace Vulkan
} // namespace Detail
} // namespace Pomdog
