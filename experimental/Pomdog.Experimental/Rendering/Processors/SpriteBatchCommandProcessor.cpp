// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/Rendering/Processors/SpriteBatchCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Commands/SpriteBatchCommand.hpp"

namespace Pomdog {
namespace Rendering {

SpriteBatchCommandProcessor::SpriteBatchCommandProcessor(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    AssetManager & assets)
    : spriteBatch(graphicsDevice, assets)
{
}

void SpriteBatchCommandProcessor::Begin(
    const std::shared_ptr<GraphicsCommandList>& commandList,
    const Matrix4x4& viewProjection)
{
    spriteBatch.Begin(commandList, viewProjection);
}

void SpriteBatchCommandProcessor::Draw(
    const std::shared_ptr<GraphicsCommandList>& commandList,
    RenderCommand & renderCommand)
{
    auto & command = static_cast<SpriteRenderCommand &>(renderCommand);
    command.Execute(spriteBatch);
}

void SpriteBatchCommandProcessor::FlushBatch()
{
    // FIXME
    // spriteBatch.Flush();
}

void SpriteBatchCommandProcessor::End()
{
    spriteBatch.End();
}

int SpriteBatchCommandProcessor::GetDrawCallCount() const noexcept
{
    return spriteBatch.GetDrawCallCount();
}

std::type_index SpriteBatchCommandProcessor::GetCommandType() const noexcept
{
    std::type_index index = typeid(SpriteBatchCommand);
    return index;
}

} // namespace Rendering
} // namespace Pomdog
