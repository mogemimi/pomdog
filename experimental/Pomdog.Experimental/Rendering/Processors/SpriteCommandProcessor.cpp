//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "SpriteCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Commands/SpriteCommand.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
SpriteCommandProcessor::SpriteCommandProcessor(std::shared_ptr<GraphicsContext> const& graphicsContext,
	std::shared_ptr<GraphicsDevice> const& graphicsDevice)
	: spriteBatch(graphicsContext, graphicsDevice)
	, drawCallCount(0)
{
}
//-----------------------------------------------------------------------
void SpriteCommandProcessor::Begin(GraphicsContext & graphicsContext)
{
	drawCallCount = 0;
	
	spriteBatch.Begin(Matrix4x4::Identity);
}
//-----------------------------------------------------------------------
void SpriteCommandProcessor::Draw(GraphicsContext & graphicsContext, RenderCommand & command)
{
	using Details::Rendering::SpriteCommand;

	auto & spriteCommand = static_cast<SpriteCommand &>(command);
	spriteBatch.Draw(spriteCommand.texture, spriteCommand.transform,
		spriteCommand.textureRegion.Subrect, spriteCommand.color, spriteCommand.originPivot);
}
//-----------------------------------------------------------------------
void SpriteCommandProcessor::End(GraphicsContext & graphicsContext)
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
}// namespace Pomdog
