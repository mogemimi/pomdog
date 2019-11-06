// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/Rendering/Processors/PrimitivePolygonCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Commands/PrimitivePolygonCommand.hpp"

namespace Pomdog {
namespace Rendering {

PrimitivePolygonCommandProcessor::PrimitivePolygonCommandProcessor(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    AssetManager& assets)
    : primitiveBatch(graphicsDevice, assets)
{
}

void PrimitivePolygonCommandProcessor::Begin(
    const std::shared_ptr<GraphicsCommandList>& commandList,
    const Matrix4x4& viewProjection)
{
    primitiveBatch.Begin(commandList, viewProjection);
}

void PrimitivePolygonCommandProcessor::Draw(
    const std::shared_ptr<GraphicsCommandList>& commandList,
    RenderCommand& renderCommand)
{
    auto& command = static_cast<PrimitivePolygonCommand&>(renderCommand);
    command.Execute(primitiveBatch);
}

void PrimitivePolygonCommandProcessor::FlushBatch()
{
    primitiveBatch.Flush();
}

void PrimitivePolygonCommandProcessor::End()
{
    primitiveBatch.End();
}

int PrimitivePolygonCommandProcessor::GetDrawCallCount() const noexcept
{
    return primitiveBatch.GetDrawCallCount();
}

std::type_index PrimitivePolygonCommandProcessor::GetCommandType() const noexcept
{
    std::type_index index = typeid(PrimitivePolygonCommand);
    return index;
}

} // namespace Rendering
} // namespace Pomdog
