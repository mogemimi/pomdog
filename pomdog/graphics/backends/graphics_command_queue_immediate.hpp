// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/GraphicsCommandQueue.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {

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

} // namespace Pomdog::Detail
