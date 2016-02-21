// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativeBuffer.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog {
namespace Detail {
namespace Vulkan {

class BufferVulkan final : public NativeBuffer {
public:
    BufferVulkan(
        ::VkDevice device,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        VkBufferUsageFlags usageFlags);

    BufferVulkan(
        ::VkDevice device,
        void const* sourceData,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        VkBufferUsageFlags usageFlags);

    ~BufferVulkan();

    void GetData(
        std::size_t offsetInBytes,
        void* destination,
        std::size_t sizeInBytes) const override;

    void SetData(
        std::size_t offsetInBytes,
        void const* source,
        std::size_t sizeInBytes) override;

    VkBuffer GetBuffer() const;

private:
    ::VkDevice device;
    ::VkBuffer nativeBuffer;
    ::VkDeviceMemory deviceMemory;
    ::VkDescriptorBufferInfo descriptor;
};

} // namespace Vulkan
} // namespace Detail
} // namespace Pomdog
