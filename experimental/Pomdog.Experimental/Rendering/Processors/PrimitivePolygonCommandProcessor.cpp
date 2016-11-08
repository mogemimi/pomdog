// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/Rendering/Processors/PrimitivePolygonCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Commands/PrimitivePolygonCommand.hpp"

namespace Pomdog {
namespace Rendering {

PrimitivePolygonCommandProcessor::PrimitivePolygonCommandProcessor(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    AssetManager & assets)
    : polygonBatch(graphicsDevice, assets)
{
}

void PrimitivePolygonCommandProcessor::Begin(
    const std::shared_ptr<GraphicsCommandList>& commandList,
    const Matrix4x4& viewProjection)
{
    polygonBatch.Begin(commandList, viewProjection);
}

void PrimitivePolygonCommandProcessor::Draw(
    const std::shared_ptr<GraphicsCommandList>& commandList,
    RenderCommand & renderCommand)
{
    auto & command = static_cast<PrimitivePolygonCommand &>(renderCommand);
    command.Execute(polygonBatch);
}

void PrimitivePolygonCommandProcessor::FlushBatch()
{
    polygonBatch.Flush();
}

void PrimitivePolygonCommandProcessor::End()
{
    polygonBatch.End();
}

int PrimitivePolygonCommandProcessor::GetDrawCallCount() const noexcept
{
    return polygonBatch.GetDrawCallCount();
}

std::type_index PrimitivePolygonCommandProcessor::GetCommandType() const noexcept
{
    std::type_index index = typeid(PrimitivePolygonCommand);
    return index;
}

} // namespace Rendering
} // namespace Pomdog
