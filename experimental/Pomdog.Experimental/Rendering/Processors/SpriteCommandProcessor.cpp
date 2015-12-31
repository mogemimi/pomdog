// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "SpriteCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Commands/SpriteCommand.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
SpriteCommandProcessor::SpriteCommandProcessor(
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    AssetManager & assets)
    : spriteBatch(graphicsDevice, assets)
    , drawCallCount(0)
{
}
//-----------------------------------------------------------------------
void SpriteCommandProcessor::Begin(GraphicsCommandQueue &)
{
    drawCallCount = 0;
    spriteBatch.Begin(Matrix4x4::Identity);
}
//-----------------------------------------------------------------------
void SpriteCommandProcessor::Draw(GraphicsCommandQueue & commandQueue, RenderCommand & command)
{
    using Detail::Rendering::SpriteCommand;

    auto & spriteCommand = static_cast<SpriteCommand &>(command);
    spriteBatch.Draw(spriteCommand.texture, spriteCommand.transform,
        spriteCommand.textureRegion.Subrect, spriteCommand.color, spriteCommand.originPivot);
}
//-----------------------------------------------------------------------
void SpriteCommandProcessor::End(GraphicsCommandQueue & commandQueue)
{
    spriteBatch.End();
    drawCallCount += spriteBatch.DrawCallCount();
}
//-----------------------------------------------------------------------
int SpriteCommandProcessor::DrawCallCount() const
{
    return drawCallCount;
}
//-----------------------------------------------------------------------
void SpriteCommandProcessor::SetViewProjection(Matrix4x4 const& view, Matrix4x4 const& projection)
{
    auto viewProjection = view * projection;
    spriteBatch.SetProjectionMatrix(viewProjection);
}
//-----------------------------------------------------------------------
} // namespace Pomdog
