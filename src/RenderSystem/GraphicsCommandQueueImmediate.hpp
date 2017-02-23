// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "NativeGraphicsCommandQueue.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>
#include <vector>

namespace Pomdog {
namespace Detail {

class GraphicsCommandListImmediate;
class NativeGraphicsContext;

class GraphicsCommandQueueImmediate final : public NativeGraphicsCommandQueue {
public:
    GraphicsCommandQueueImmediate() = delete;
    GraphicsCommandQueueImmediate(const GraphicsCommandQueueImmediate&) = delete;
    GraphicsCommandQueueImmediate & operator=(const GraphicsCommandQueueImmediate&) = delete;

    explicit GraphicsCommandQueueImmediate(
        const std::shared_ptr<NativeGraphicsContext>& graphicsContext);

    void Reset() override;

    void PushbackCommandList(
        const std::shared_ptr<GraphicsCommandList>& commandList) override;

    void ExecuteCommandLists() override;

    void Present() override;

    std::size_t GetCommandCount() const noexcept override;

private:
    std::vector<std::shared_ptr<GraphicsCommandListImmediate>> commandLists;
    std::shared_ptr<NativeGraphicsContext> graphicsContext;
};

} // namespace Detail
} // namespace Pomdog
