//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "Renderer.hpp"
#include "RenderCommand.hpp"
#include "../Graphics/SpriteRenderer.hpp"

//#include "Graphics/PolygonBatch.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
Renderer::Renderer(std::shared_ptr<GraphicsContext> const& graphicsContext,
	std::shared_ptr<GraphicsDevice> const& graphicsDevice, AssetManager & assets)
{
	spriteRenderer = std::make_unique<SpriteRenderer>(graphicsContext, graphicsDevice, assets);
	//spriteBatch = std::make_unique<SpriteBatch>(graphicsContext, graphicsDevice, *assets);
	//polygonBatch = std::make_unique<PolygonBatch>(graphicsContext, graphicsDevice, *assets);
	//spriteRenderer->SetProjectionMatrix(Matrix4x4::CreateOrthographicLH(bounds.Width, bounds.Height, 1.0f, 100.0f));
}
//-----------------------------------------------------------------------
void Renderer::Render(std::shared_ptr<GraphicsContext> const& graphicsContext)
{
	renderQueue.Sort();
	renderQueue.Enumerate([&](RenderCommand & command)
	{
		switch (command.CommandType()) {
		case RenderCommandType::Custom:
		case RenderCommandType::Batch: {
			command.Execute(graphicsContext);
			break;
		}
		}
	});
	renderQueue.Clear();
}
//-----------------------------------------------------------------------
Renderer::~Renderer() = default;
//-----------------------------------------------------------------------
}// namespace Pomdog
