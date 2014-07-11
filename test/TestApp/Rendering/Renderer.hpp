//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_RENDERER_1196526B_9C0E_408B_A501_2A5D941209ED_HPP
#define POMDOG_RENDERER_1196526B_9C0E_408B_A501_2A5D941209ED_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>
#include "RenderQueue.hpp"

namespace Pomdog {

class SpriteRenderer;
class PolygonBatch;

class Renderer {
public:
	Renderer(std::shared_ptr<GraphicsContext> const& graphicsContext,
		std::shared_ptr<GraphicsDevice> const& graphicsDevice, AssetManager & assets);

	~Renderer();

	void Render(std::shared_ptr<GraphicsContext> const& graphicsContext);
	
public:
	RenderQueue renderQueue;
	std::unique_ptr<SpriteRenderer> spriteRenderer;
	//std::unique_ptr<PolygonBatch> polygonBatch;
	//std::unique_ptr<SpriteBatch> spriteBatch;
};

}// namespace Pomdog

#endif // !defined(POMDOG_RENDERER_1196526B_9C0E_408B_A501_2A5D941209ED_HPP)
