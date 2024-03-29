// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/command_queue.h"
#include "pomdog/gpu/forward_declarations.h"
#include <vulkan/vulkan.h>

namespace pomdog::gpu::detail::vulkan {

class CommandQueueVulkan final : public CommandQueue {
public:
    void Reset();

    void pushBackCommandList(const std::shared_ptr<CommandList>& commandList);

    void ExecuteCommandLists();

    void Present();

    std::size_t GetCommandCount() const noexcept;

private:
};

} // namespace pomdog::gpu::detail::vulkan
