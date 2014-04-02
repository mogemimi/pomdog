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
	
	~SpriteRenderer();
	
	void Begin(Matrix3x3 const& transformMatrix);
	
	void Draw(std::shared_ptr<Texture2D> const& texture,
		Rectangle const& sourceRect, Color const& color);
		
	void Draw(std::shared_ptr<Texture2D> const& texture,
		Vector2 const& position, Color const& color);
	
	void Draw(std::shared_ptr<Texture2D> const& texture,
		Vector2 const& position, Rectangle const& sourceRect, Color const& color);
	
	void Draw(std::shared_ptr<Texture2D> const& texture,
		Vector2 const& position, Rectangle const& sourceRect, Color const& color,
		Radian<float> const& rotation, Vector2 const& originPivot, float scale, float layerDepth);
	
	void Draw(std::shared_ptr<Texture2D> const& texture,
		Vector2 const& position, Rectangle const& sourceRect, Color const& color,
		Radian<float> const& rotation, Vector2 const& originPivot, Vector2 const& scale, float layerDepth);
		
	void End();
	
private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace TestApp

#endif // !defined(POMDOG_SPRITERENDERER_8ACAC405_03FF_42D3_87CA_D25B183EF883_HPP)
