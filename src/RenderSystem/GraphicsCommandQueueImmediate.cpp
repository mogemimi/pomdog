// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "GraphicsCommandQueueImmediate.hpp"
#include "GraphicsCommandListImmediate.hpp"
#include "GraphicsContext.hpp"
#include "Pomdog/Graphics/GraphicsCommandList.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
//-----------------------------------------------------------------------
GraphicsCommandQueueImmediate::GraphicsCommandQueueImmediate(std::shared_ptr<Detail::GraphicsContext> const& graphicsContextIn)
    : graphicsContext(graphicsContextIn)
{
}
//-----------------------------------------------------------------------
void GraphicsCommandQueueImmediate::Reset()
{
    commandLists.clear();
}
//-----------------------------------------------------------------------
void GraphicsCommandQueueImmediate::PushbackCommandList(
    std::shared_ptr<GraphicsCommandList> const& commandList)
{
    POMDOG_ASSERT(commandList);
    if (commandList) {
        commandLists.push_back(commandList);
    }
}
//-----------------------------------------------------------------------
void GraphicsCommandQueueImmediate::ExecuteCommandLists()
{
    for (auto & commandList : commandLists) {
        POMDOG_ASSERT(graphicsContext);
        POMDOG_ASSERT(commandList);

        ///@todo badcode
        auto commandListImmediate = dynamic_cast<Detail::GraphicsCommandListImmediate*>(commandList->GetNativeGraphicsCommandList());
        if (commandListImmediate) {
            commandListImmediate->ExecuteImmediate(*graphicsContext);
        }
    }
}
//-----------------------------------------------------------------------
void GraphicsCommandQueueImmediate::Present()
{
    POMDOG_ASSERT(graphicsContext);
    graphicsContext->Present();
}
//-----------------------------------------------------------------------
std::size_t GraphicsCommandQueueImmediate::GetCommandCount() const noexcept
{
    std::size_t count = 0;
    for (auto & commandList : commandLists) {
        POMDOG_ASSERT(commandList);
        count += commandList->GetCount();
    }
    return count;
}
//-----------------------------------------------------------------------
} // namespace Detail
} // namespace Pomdog
