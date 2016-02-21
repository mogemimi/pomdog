// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/command_queue.h"
#include "pomdog/gpu/vulkan/prerequisites_vulkan.h"
#include "pomdog/memory/unsafe_ptr.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class CommandList;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::vulkan {
class CommandListVulkan;
class SwapChainVulkan;
} // namespace pomdog::gpu::detail::vulkan

namespace pomdog::gpu::detail::vulkan {

class CommandQueueVulkan final : public CommandQueue {
private:
    ::VkDevice device_ = nullptr;
    ::VkQueue graphicsQueue_ = nullptr;
    ::VkFence submitFence_ = nullptr;
    unsafe_ptr<SwapChainVulkan> swapChain_ = nullptr;
    std::vector<std::shared_ptr<CommandListVulkan>> commandLists_;

public:
    CommandQueueVulkan() = delete;

    explicit CommandQueueVulkan(
        ::VkDevice device,
        ::VkQueue graphicsQueue);

    ~CommandQueueVulkan() override;

    void reset() override;

    void pushBackCommandList(const std::shared_ptr<CommandList>& commandList) override;

    void executeCommandLists() override;

    void present() override;

    /// Sets the swap chain for presentation.
    void setSwapChain(unsafe_ptr<SwapChainVulkan> swapChain) noexcept;
};

} // namespace pomdog::gpu::detail::vulkan
