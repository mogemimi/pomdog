// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "PrimitiveCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Commands/PrimitiveCommand.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
PrimitiveCommandProcessor::PrimitiveCommandProcessor(
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    AssetManager & assets)
    : primitiveBatch(graphicsDevice, assets)
    , drawCallCount(0)
{
}
//-----------------------------------------------------------------------
void PrimitiveCommandProcessor::Begin(GraphicsCommandQueue & commandQueue)
{
    drawCallCount = 0;
    primitiveBatch.Begin(viewProjection);
}
//-----------------------------------------------------------------------
void PrimitiveCommandProcessor::Draw(
    GraphicsCommandQueue & commandQueue, RenderCommand & command)
{
    using Detail::Rendering::PrimitiveCommand;

    auto & primitiveCommand = static_cast<PrimitiveCommand &>(command);
    primitiveBatch.DrawRectangle(
        primitiveCommand.transform,
        primitiveCommand.rectangle,
        primitiveCommand.leftBottomColor,
        primitiveCommand.rightBottomColor,
        primitiveCommand.rightTopColor,
        primitiveCommand.leftTopColor);
}
//-----------------------------------------------------------------------
void PrimitiveCommandProcessor::End(GraphicsCommandQueue & commandQueue)
{
    primitiveBatch.End();

    drawCallCount += primitiveBatch.GetDrawCallCount();
}
//-----------------------------------------------------------------------
int PrimitiveCommandProcessor::GetDrawCallCount() const noexcept
{
    return drawCallCount;
}
//-----------------------------------------------------------------------
void PrimitiveCommandProcessor::SetViewProjection(
    Matrix4x4 const& view, Matrix4x4 const& projection)
{
    viewProjection = view * projection;
}
//-----------------------------------------------------------------------
} // namespace Pomdog
