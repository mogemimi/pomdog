// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "GraphicsCommandQueueVulkan.hpp"
#include "../Utility/Exception.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog::Detail::Vulkan {

void GraphicsCommandQueueVulkan::Reset()
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GraphicsCommandQueueVulkan::PushbackCommandList(const std::shared_ptr<GraphicsCommandList>& commandList)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GraphicsCommandQueueVulkan::ExecuteCommandLists()
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GraphicsCommandQueueVulkan::Present()
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

std::size_t GraphicsCommandQueueVulkan::GetCommandCount() const noexcept
{
    // FIXME: Not implemented yet.
    return 0;
}

} // namespace Pomdog::Detail::Vulkan
