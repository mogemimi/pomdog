// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/backends/command_queue_immediate.h"
#include "pomdog/gpu/backends/command_list_immediate.h"
#include "pomdog/gpu/backends/graphics_context.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/utility/assert.h"

namespace pomdog::gpu::detail {

CommandQueueImmediate::CommandQueueImmediate(
    const std::shared_ptr<GraphicsContext>& graphicsContextIn)
    : graphicsContext_(graphicsContextIn)
{
}

void CommandQueueImmediate::reset()
{
    commandLists_.clear();
}

void CommandQueueImmediate::pushBackCommandList(
    const std::shared_ptr<CommandList>& commandList)
{
    POMDOG_ASSERT(commandList);
    if (commandList) {
        auto commandListImmediate = std::static_pointer_cast<CommandListImmediate>(commandList);
        POMDOG_ASSERT(commandListImmediate != nullptr);
        POMDOG_ASSERT(commandListImmediate == std::dynamic_pointer_cast<CommandListImmediate>(commandList));
        commandLists_.push_back(std::move(commandListImmediate));
    }
}

void CommandQueueImmediate::executeCommandLists()
{
    graphicsContext_->executeCommandLists(commandLists_);
}

void CommandQueueImmediate::present()
{
    POMDOG_ASSERT(graphicsContext_);
    graphicsContext_->present();
}

} // namespace pomdog::gpu::detail
