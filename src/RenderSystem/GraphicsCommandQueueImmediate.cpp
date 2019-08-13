// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

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
        auto commandListImmediate = dynamic_cast<GraphicsCommandListImmediate*>(
            commandList->GetNativeGraphicsCommandList());
        std::shared_ptr<GraphicsCommandListImmediate> shared(commandList, commandListImmediate);
        commandLists.push_back(shared);
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
    for (auto & commandList : commandLists) {
        POMDOG_ASSERT(commandList);
        count += commandList->GetCount();
    }
    return count;
}

} // namespace Pomdog::Detail
