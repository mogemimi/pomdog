// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativeGraphicsCommandQueue.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog::Detail::Vulkan {

class GraphicsCommandQueueVulkan final : public NativeGraphicsCommandQueue {
public:
    void Reset();

    void PushbackCommandList(const std::shared_ptr<GraphicsCommandList>& commandList);

    void ExecuteCommandLists();

    void Present();

    std::size_t GetCommandCount() const noexcept;

private:
};

} // namespace Pomdog::Detail::Vulkan
