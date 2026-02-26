// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vulkan/command_queue_vulkan.h"
#include "pomdog/gpu/vulkan/command_list_vulkan.h"
#include "pomdog/utility/assert.h"

namespace pomdog::gpu::detail::vulkan {

void CommandQueueVulkan::reset()
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void CommandQueueVulkan::pushBackCommandList(const std::shared_ptr<CommandList>& commandList)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void CommandQueueVulkan::executeCommandLists()
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void CommandQueueVulkan::present()
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

std::size_t CommandQueueVulkan::getCommandCount() const noexcept
{
    // FIXME: Not implemented yet.
    return 0;
}

} // namespace pomdog::gpu::detail::vulkan
