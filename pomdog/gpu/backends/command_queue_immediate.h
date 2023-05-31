// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/command_queue.h"
#include "pomdog/gpu/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail {

class CommandListImmediate;
class GraphicsContext;

class CommandQueueImmediate final : public CommandQueue {
public:
    CommandQueueImmediate() = delete;
    CommandQueueImmediate(const CommandQueueImmediate&) = delete;
    CommandQueueImmediate& operator=(const CommandQueueImmediate&) = delete;

    explicit CommandQueueImmediate(
        const std::shared_ptr<GraphicsContext>& graphicsContext);

    void reset() override;

    void pushBackCommandList(const std::shared_ptr<CommandList>& commandList) override;

    void executeCommandLists() override;

    void present() override;

private:
    std::vector<std::shared_ptr<CommandListImmediate>> commandLists_;
    std::shared_ptr<GraphicsContext> graphicsContext_;
};

} // namespace pomdog::gpu::detail
