// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vulkan/command_queue_vulkan.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/exception.h"

namespace pomdog::gpu::detail::vulkan {

void CommandQueueVulkan::Reset()
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void CommandQueueVulkan::PushbackCommandList(const std::shared_ptr<CommandList>& commandList)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void CommandQueueVulkan::ExecuteCommandLists()
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void CommandQueueVulkan::Present()
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

std::size_t CommandQueueVulkan::GetCommandCount() const noexcept
{
    // FIXME: Not implemented yet.
    return 0;
}

} // namespace pomdog::gpu::detail::vulkan
