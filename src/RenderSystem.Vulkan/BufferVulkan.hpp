// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/BufferBindMode.hpp"
#include "../RenderSystem/NativeBuffer.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vulkan/vulkan.h>
#include <cstddef>
#include <cstdint>
#include <tuple>

namespace Pomdog::Detail::Vulkan {

class BufferVulkan final : public NativeBuffer {
public:
    BufferVulkan(
        ::VkDevice device,
        ::VkPhysicalDevice physicalDevice,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        BufferBindMode bindMode);

    BufferVulkan(
        ::VkDevice device,
        ::VkPhysicalDevice physicalDevice,
        const void* sourceData,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        BufferBindMode bindMode);

    ~BufferVulkan();

    void GetData(
        std::size_t offsetInBytes,
        void* destination,
        std::size_t sizeInBytes) const override;

    void SetData(
        std::size_t offsetInBytes,
        const void* source,
        std::size_t sizeInBytes) override;

    VkBuffer GetBuffer() const;

private:
    ::VkDevice device;
    ::VkBuffer nativeBuffer;
    ::VkDeviceMemory nativeBufferMemory;
};

std::tuple<VkBuffer, VkDeviceMemory>
CreateBuffer(
    ::VkDevice device,
    ::VkPhysicalDevice physicalDevice,
    std::size_t sizeInBytes,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags propertyFlags);

} // namespace Pomdog::Detail::Vulkan
