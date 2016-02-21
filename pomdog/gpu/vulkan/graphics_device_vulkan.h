// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/vulkan/prerequisites_vulkan.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::vulkan {
class SwapChainVulkan;
} // namespace pomdog::gpu::detail::vulkan

namespace pomdog::gpu::detail::vulkan {

class GraphicsDeviceVulkan : public GraphicsDevice {
public:
    ~GraphicsDeviceVulkan() override = default;

    /// Creates a new GraphicsDeviceVulkan instance.
    [[nodiscard]] static std::tuple<std::shared_ptr<GraphicsDeviceVulkan>, std::unique_ptr<Error>>
    create() noexcept;

    /// Gets the Vulkan device.
    [[nodiscard]] virtual ::VkDevice
    getDevice() const noexcept = 0;

    /// Gets the Vulkan graphics queue.
    [[nodiscard]] virtual ::VkQueue
    getGraphicsQueue() const noexcept = 0;

    /// Gets the Vulkan instance.
    [[nodiscard]] virtual ::VkInstance
    getInstance() const noexcept = 0;

    /// Gets the Vulkan physical device.
    [[nodiscard]] virtual ::VkPhysicalDevice
    getPhysicalDevice() const noexcept = 0;

    /// Gets the Vulkan command pool.
    [[nodiscard]] virtual ::VkCommandPool
    getCommandPool() const noexcept = 0;

    /// Gets the graphics queue family index.
    [[nodiscard]] virtual u32
    getGraphicsQueueFamilyIndex() const noexcept = 0;

    /// Sets the swap chain for automatic configuration of new command lists.
    virtual void
    setSwapChain(unsafe_ptr<SwapChainVulkan> swapChain) noexcept = 0;

    /// Sets the presentation parameters.
    virtual void
    setPresentationParameters(const PresentationParameters& params) noexcept = 0;
};

} // namespace pomdog::gpu::detail::vulkan
