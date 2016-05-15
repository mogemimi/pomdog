// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "NativeGraphicsCommandQueue.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>
#include <vector>

namespace Pomdog {
namespace Detail {

class GraphicsContext;

class GraphicsCommandQueueImmediate final : public NativeGraphicsCommandQueue {
public:
    GraphicsCommandQueueImmediate() = delete;
    GraphicsCommandQueueImmediate(const GraphicsCommandQueueImmediate&) = delete;
    GraphicsCommandQueueImmediate & operator=(const GraphicsCommandQueueImmediate&) = delete;

    explicit GraphicsCommandQueueImmediate(
        const std::shared_ptr<GraphicsContext>& graphicsContext);

    void Reset() override;

    void PushbackCommandList(
        const std::shared_ptr<GraphicsCommandList>& commandList) override;

    void ExecuteCommandLists() override;

    void Present() override;

    std::size_t GetCommandCount() const noexcept override;

private:
    std::vector<std::shared_ptr<GraphicsCommandList>> commandLists;
    std::shared_ptr<GraphicsContext> graphicsContext;
};

} // namespace Detail
} // namespace Pomdog
