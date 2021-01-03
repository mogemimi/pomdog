// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "GraphicsCommandQueueImmediate.hpp"
#include "GraphicsCommandListImmediate.hpp"
#include "NativeGraphicsContext.hpp"
#include "Pomdog/Graphics/GraphicsCommandList.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog::Detail {

GraphicsCommandQueueImmediate::GraphicsCommandQueueImmediate(
    const std::shared_ptr<NativeGraphicsContext>& graphicsContextIn)
    : graphicsContext(graphicsContextIn)
{
}

void GraphicsCommandQueueImmediate::Reset()
{
    commandLists.clear();
}

void GraphicsCommandQueueImmediate::PushbackCommandList(
    const std::shared_ptr<GraphicsCommandList>& commandList)
{
    POMDOG_ASSERT(commandList);
    if (commandList) {
        auto commandListImmediate = std::static_pointer_cast<GraphicsCommandListImmediate>(commandList);
        POMDOG_ASSERT(commandListImmediate != nullptr);
        POMDOG_ASSERT(commandListImmediate == std::dynamic_pointer_cast<GraphicsCommandListImmediate>(commandList));
        commandLists.push_back(std::move(commandListImmediate));
    }
}

void GraphicsCommandQueueImmediate::ExecuteCommandLists()
{
    graphicsContext->ExecuteCommandLists(commandLists);
}

void GraphicsCommandQueueImmediate::Present()
{
    POMDOG_ASSERT(graphicsContext);
    graphicsContext->Present();
}

std::size_t GraphicsCommandQueueImmediate::GetCommandCount() const noexcept
{
    std::size_t count = 0;
    for (auto& commandList : commandLists) {
        POMDOG_ASSERT(commandList);
        count += commandList->GetCount();
    }
    return count;
}

} // namespace Pomdog::Detail
