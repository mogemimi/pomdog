// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "NativeGraphicsCommandQueue.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>
#include <vector>

namespace Pomdog {
namespace Detail {

class GraphicsContext;

class GraphicsCommandQueueImmediate final: public NativeGraphicsCommandQueue {
public:
    GraphicsCommandQueueImmediate() = delete;
    GraphicsCommandQueueImmediate(GraphicsCommandQueueImmediate const&) = delete;
    GraphicsCommandQueueImmediate & operator=(GraphicsCommandQueueImmediate const&) = delete;

    explicit GraphicsCommandQueueImmediate(std::shared_ptr<GraphicsContext> const& graphicsContext);

    void Reset() override;

    void PushbackCommandList(std::shared_ptr<GraphicsCommandList> const& commandList) override;

    void ExecuteCommandLists() override;

    void Present() override;

    std::size_t GetCommandCount() const noexcept override;

private:
    std::vector<std::shared_ptr<GraphicsCommandList>> commandLists;
    std::shared_ptr<GraphicsContext> graphicsContext;
};

} // namespace Detail
} // namespace Pomdog
