// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/graphics_command_queue.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog::Detail::Vulkan {

class GraphicsCommandQueueVulkan final : public GraphicsCommandQueue {
public:
    void Reset();

    void PushbackCommandList(const std::shared_ptr<GraphicsCommandList>& commandList);

    void ExecuteCommandLists();

    void Present();

    std::size_t GetCommandCount() const noexcept;

private:
};

} // namespace Pomdog::Detail::Vulkan
