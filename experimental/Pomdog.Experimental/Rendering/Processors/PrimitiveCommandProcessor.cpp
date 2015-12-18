// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include "PrimitiveCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Commands/PrimitiveCommand.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
PrimitiveCommandProcessor::PrimitiveCommandProcessor(
    std::shared_ptr<GraphicsContext> const& graphicsContext,
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    AssetManager & assets)
    : primitiveBatch(graphicsContext, graphicsDevice, assets)
    , drawCallCount(0)
{
}
//-----------------------------------------------------------------------
void PrimitiveCommandProcessor::Begin(GraphicsContext & graphicsContext)
{
    drawCallCount = 0;

    primitiveBatch.Begin(viewProjection);
}
//-----------------------------------------------------------------------
void PrimitiveCommandProcessor::Draw(GraphicsContext & graphicsContext, RenderCommand & command)
{
    using Detail::Rendering::PrimitiveCommand;

    auto & primitiveCommand = static_cast<PrimitiveCommand &>(command);
    primitiveBatch.DrawRectangle(primitiveCommand.transform,
        primitiveCommand.rectangle,
        primitiveCommand.leftBottomColor, primitiveCommand.rightBottomColor,
        primitiveCommand.rightTopColor, primitiveCommand.leftTopColor);
}
//-----------------------------------------------------------------------
void PrimitiveCommandProcessor::End(GraphicsContext & graphicsContext)
{
    primitiveBatch.End();

    drawCallCount += primitiveBatch.DrawCallCount();
}
//-----------------------------------------------------------------------
int PrimitiveCommandProcessor::DrawCallCount() const
{
    return drawCallCount;
}
//-----------------------------------------------------------------------
void PrimitiveCommandProcessor::SetViewProjection(Matrix4x4 const& view, Matrix4x4 const& projection)
{
    viewProjection = view * projection;
}
//-----------------------------------------------------------------------
} // namespace Pomdog
