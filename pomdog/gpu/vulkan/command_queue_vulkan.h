// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/command_queue.h"
#include "pomdog/gpu/vulkan/prerequisites_vulkan.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class CommandList;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::vulkan {
class CommandListVulkan;
} // namespace pomdog::gpu::detail::vulkan

namespace pomdog::gpu::detail::vulkan {

class CommandQueueVulkan final : public CommandQueue {
public:
    void reset();

    void pushBackCommandList(const std::shared_ptr<CommandList>& commandList);

    void executeCommandLists();

    void present();

    std::size_t getCommandCount() const noexcept;

private:
};

} // namespace pomdog::gpu::detail::vulkan
