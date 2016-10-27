// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "SpriteBatchCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Commands/SpriteBatchCommand.hpp"

namespace Pomdog {
namespace Rendering {

SpriteBatchCommandProcessor::SpriteBatchCommandProcessor(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    AssetManager & assets)
    : spriteBatch(graphicsDevice, assets)
    , viewProjection(Matrix4x4::Identity)
    , drawCallCount(0)
{
}

void SpriteBatchCommandProcessor::Begin(
    const std::shared_ptr<GraphicsCommandList>& commandList)
{
    drawCallCount = 0;
    spriteBatch.Begin(commandList, viewProjection);
}

void SpriteBatchCommandProcessor::Draw(
    const std::shared_ptr<GraphicsCommandList>& commandList,
    RenderCommand & command)
{
    using Pomdog::Rendering::SpriteBatchCommand;
    auto & spriteCommand = static_cast<SpriteBatchCommand &>(command);
    if (spriteCommand.onDraw) {
        spriteCommand.onDraw(spriteBatch);
    }
}

void SpriteBatchCommandProcessor::End(
    const std::shared_ptr<GraphicsCommandList>& commandList)
{
    spriteBatch.End();
    drawCallCount += spriteBatch.GetDrawCallCount();
}

int SpriteBatchCommandProcessor::GetDrawCallCount() const noexcept
{
    return drawCallCount;
}

void SpriteBatchCommandProcessor::SetViewProjection(
    const Matrix4x4& view, const Matrix4x4& projection)
{
    viewProjection = view * projection;
}

std::type_index SpriteBatchCommandProcessor::GetCommandType() const noexcept
{
    std::type_index index = typeid(SpriteBatchCommand);
    return index;
}

} // namespace Rendering
} // namespace Pomdog
