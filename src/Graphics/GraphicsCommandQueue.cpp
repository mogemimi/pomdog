// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/GraphicsCommandQueue.hpp"
#include "../RenderSystem/NativeGraphicsCommandQueue.hpp"
#include "Pomdog/Graphics/GraphicsCommandList.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {

GraphicsCommandQueue::GraphicsCommandQueue(
    std::unique_ptr<Detail::NativeGraphicsCommandQueue> && nativeCommandQueueIn)
    : nativeCommandQueue(std::move(nativeCommandQueueIn))
{
}

void GraphicsCommandQueue::Reset()
{
    POMDOG_ASSERT(nativeCommandQueue);
    nativeCommandQueue->Reset();
}

void GraphicsCommandQueue::PushbackCommandList(
    const std::shared_ptr<GraphicsCommandList>& commandList)
{
    POMDOG_ASSERT(nativeCommandQueue);
    POMDOG_ASSERT(commandList);
    if (commandList) {
        nativeCommandQueue->PushbackCommandList(commandList);
    }
}

void GraphicsCommandQueue::ExecuteCommandLists()
{
    POMDOG_ASSERT(nativeCommandQueue);
    nativeCommandQueue->ExecuteCommandLists();
}

void GraphicsCommandQueue::Present()
{
    POMDOG_ASSERT(nativeCommandQueue);
    nativeCommandQueue->Present();
}

std::size_t GraphicsCommandQueue::GetCommandCount() const noexcept
{
    POMDOG_ASSERT(nativeCommandQueue);
    return nativeCommandQueue->GetCommandCount();
}

} // namespace Pomdog
