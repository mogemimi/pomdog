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

class GraphicsCommandListImmediate;
class NativeGraphicsContext;

class GraphicsCommandQueueImmediate final : public GraphicsCommandQueue {
public:
    GraphicsCommandQueueImmediate() = delete;
    GraphicsCommandQueueImmediate(const GraphicsCommandQueueImmediate&) = delete;
    GraphicsCommandQueueImmediate& operator=(const GraphicsCommandQueueImmediate&) = delete;

    explicit GraphicsCommandQueueImmediate(
        const std::shared_ptr<NativeGraphicsContext>& graphicsContext);

    std::size_t GetCommandCount() const noexcept override;

    void Reset() override;

    void PushbackCommandList(const std::shared_ptr<GraphicsCommandList>& commandList) override;

    void ExecuteCommandLists() override;

    void Present() override;

private:
    std::vector<std::shared_ptr<GraphicsCommandListImmediate>> commandLists;
    std::shared_ptr<NativeGraphicsContext> graphicsContext;
};

} // namespace pomdog::gpu::detail
