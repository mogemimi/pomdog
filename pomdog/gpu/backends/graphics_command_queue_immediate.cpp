// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/backends/graphics_command_queue_immediate.h"
#include "pomdog/gpu/backends/graphics_command_list_immediate.h"
#include "pomdog/gpu/backends/native_graphics_context.h"
#include "pomdog/gpu/graphics_command_list.h"
#include "pomdog/utility/assert.h"

namespace pomdog::detail {

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

} // namespace pomdog::detail
