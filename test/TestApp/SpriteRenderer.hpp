//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SPRITERENDERER_8ACAC405_03FF_42D3_87CA_D25B183EF883_HPP
#define POMDOG_SPRITERENDERER_8ACAC405_03FF_42D3_87CA_D25B183EF883_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>

namespace TestApp {

using namespace Pomdog;

class SpriteRenderer {
public:
	explicit SpriteRenderer(std::shared_ptr<GameHost> const& gameHost);
	
	void Draw(std::shared_ptr<Texture2D> const& texture,
		Vector2 const& position, Vector2 const& scale, Radian<float> const& rotation,
		Rectangle const& sourceRect, Vector2 const& originPivot, float layerDepth);
	
private:
	std::shared_ptr<GraphicsContext> graphicsContext;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;
	std::shared_ptr<EffectPass> effectPass;
	std::shared_ptr<InputLayout> inputLayout;
};

}// namespace TestApp

#endif // !defined(POMDOG_SPRITERENDERER_8ACAC405_03FF_42D3_87CA_D25B183EF883_HPP)
