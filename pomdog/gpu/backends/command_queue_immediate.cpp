// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/backends/command_queue_immediate.h"
#include "pomdog/gpu/backends/command_list_immediate.h"
#include "pomdog/gpu/backends/native_graphics_context.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/utility/assert.h"

namespace pomdog::gpu::detail {

CommandQueueImmediate::CommandQueueImmediate(
    const std::shared_ptr<NativeGraphicsContext>& graphicsContextIn)
    : graphicsContext(graphicsContextIn)
{
}

void CommandQueueImmediate::Reset()
{
    commandLists.clear();
}

void CommandQueueImmediate::PushbackCommandList(
    const std::shared_ptr<CommandList>& commandList)
{
    POMDOG_ASSERT(commandList);
    if (commandList) {
        auto commandListImmediate = std::static_pointer_cast<CommandListImmediate>(commandList);
        POMDOG_ASSERT(commandListImmediate != nullptr);
        POMDOG_ASSERT(commandListImmediate == std::dynamic_pointer_cast<CommandListImmediate>(commandList));
        commandLists.push_back(std::move(commandListImmediate));
    }
}

void CommandQueueImmediate::ExecuteCommandLists()
{
    graphicsContext->ExecuteCommandLists(commandLists);
}

void CommandQueueImmediate::Present()
{
    POMDOG_ASSERT(graphicsContext);
    graphicsContext->Present();
}

std::size_t CommandQueueImmediate::GetCommandCount() const noexcept
{
    std::size_t count = 0;
    for (auto& commandList : commandLists) {
        POMDOG_ASSERT(commandList);
        count += commandList->GetCount();
    }
    return count;
}

} // namespace pomdog::gpu::detail
