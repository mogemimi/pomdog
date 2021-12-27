// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/forward_declarations.h"
#include "pomdog/graphics/graphics_command_queue.h"
#include <vulkan/vulkan.h>

namespace pomdog::detail::vulkan {

class GraphicsCommandQueueVulkan final : public GraphicsCommandQueue {
public:
    void Reset();

    void PushbackCommandList(const std::shared_ptr<GraphicsCommandList>& commandList);

    void ExecuteCommandLists();

    void Present();

    std::size_t GetCommandCount() const noexcept;

private:
};

} // namespace pomdog::detail::vulkan
