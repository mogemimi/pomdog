//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SPRITEDRAWINGCONTEXT_4AC87153_9AFF_4B5C_AA4F_B9125DD093DE_HPP
#define POMDOG_SPRITEDRAWINGCONTEXT_4AC87153_9AFF_4B5C_AA4F_B9125DD093DE_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <Pomdog/Pomdog.hpp>
#include "UI/DrawingContext.hpp"
#include "SpriteBatch.hpp"

namespace Pomdog {
namespace UI {

class SpriteDrawingContext final: public UI::DrawingContext {
private:
	SpriteBatch & spriteBatch;
	std::shared_ptr<Texture2D> texture;
	
public:
	explicit SpriteDrawingContext(SpriteBatch & spriteBatchIn, std::shared_ptr<Texture2D> const& textureIn)
		: spriteBatch(spriteBatchIn)
		, texture(textureIn)
	{
	}
	
	void DrawRectangle(Color const& color, Rectangle const& rectangle)
	{
		spriteBatch.Draw(texture, Vector2(rectangle.X, rectangle.Y),
			Rectangle{0, 0, 1, 1}, color, 0, {0.0f, 0.0f}, Vector2(rectangle.Width, rectangle.Height), 0.0f);
	}
	
	void DrawLine(Color const& color, float penSize, Point2D const& point1, Point2D const& point2)
	{
	}
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_SPRITEDRAWINGCONTEXT_4AC87153_9AFF_4B5C_AA4F_B9125DD093DE_HPP)
