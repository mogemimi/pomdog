//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SPRITEBATCH_B4DEBA3B_98DD_4767_A79A_4CAFC8A81AF4_HPP
#define POMDOG_SPRITEBATCH_B4DEBA3B_98DD_4767_A79A_4CAFC8A81AF4_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <Pomdog/Pomdog.hpp>

namespace TestApp {

using namespace Pomdog;

class SpriteBatch {
public:
	SpriteBatch(std::shared_ptr<GraphicsContext> const& graphicsContext,
		std::shared_ptr<GraphicsDevice> const& graphicsDevice, AssetManager & assets);
	
	~SpriteBatch();
	
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

#endif // !defined(POMDOG_SPRITEBATCH_B4DEBA3B_98DD_4767_A79A_4CAFC8A81AF4_HPP)
