// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/graphics/backends/graphics_command_queue_immediate.hpp"
#include "pomdog/graphics/backends/graphics_command_list_immediate.hpp"
#include "pomdog/graphics/backends/native_graphics_context.hpp"
#include "pomdog/graphics/graphics_command_list.hpp"
#include "pomdog/utility/assert.hpp"

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
