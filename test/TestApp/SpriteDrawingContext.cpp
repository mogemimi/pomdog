//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "SpriteDrawingContext.hpp"

namespace Pomdog {
namespace UI {
//-----------------------------------------------------------------------
SpriteDrawingContext::SpriteDrawingContext(SpriteBatch & spriteBatchIn, SpriteBatch & spriteFontBatchIn,
	std::shared_ptr<EffectPass> const& distanceFieldEffectIn,
	std::shared_ptr<ConstantBufferBinding> const& constantBuffersIn,
	SpriteFont & spriteFontIn, std::shared_ptr<Texture2D> const& textureIn)
	: spriteBatch(spriteBatchIn)
	, spriteFontBatch(spriteFontBatchIn)
	, spriteFont(spriteFontIn)
	, distanceFieldEffect(distanceFieldEffectIn)
	, constantBuffers(constantBuffersIn)
	, texture(textureIn)
{}
//-----------------------------------------------------------------------
Matrix3x2 SpriteDrawingContext::Top() const
{
	POMDOG_ASSERT(!matrixStack.empty());
	return matrixStack.back();
}
//-----------------------------------------------------------------------
void SpriteDrawingContext::Push(Matrix3x2 const& matrix)
{
	matrixStack.push_back(matrix);
}
//-----------------------------------------------------------------------
void SpriteDrawingContext::Pop()
{
	POMDOG_ASSERT(!matrixStack.empty());
	matrixStack.pop_back();
}
//-----------------------------------------------------------------------
void SpriteDrawingContext::DrawRectangle(Matrix3x2 const& transform, Color const& color, Rectangle const& rectangle)
{
	///@todo Not implemented
	auto position = Vector2::Transform(Vector2(rectangle.X, rectangle.Y), transform);// badcode
	
	spriteBatch.Draw(texture, position, Rectangle{0, 0, 1, 1},
		color, 0, {0.0f, 0.0f}, Vector2(rectangle.Width, rectangle.Height), 0.0f);
}
//-----------------------------------------------------------------------
void SpriteDrawingContext::DrawLine(Matrix3x2 const& transform, Color const& color, float penSize, Vector2 const& point1, Vector2 const& point2)
{
	auto transformedPoint1 = Vector2::Transform(point1, transform);
	auto transformedPoint2 = Vector2::Transform(point2, transform);
	
	auto lineLength = Vector2::Distance(transformedPoint2, transformedPoint1);
	auto thicknessScale = penSize;
	auto tangent = transformedPoint2 - transformedPoint1;
	auto rotation = std::atan2(-tangent.Y, tangent.X);

	spriteBatch.Draw(texture, transformedPoint1 + Vector2{0.5f, 0.5f}, Rectangle{0, 0, 1, 1},
		color, rotation, {0.0f, 0.5f}, Vector2{lineLength + 0.5f, thicknessScale}, 0.0f);
}
//-----------------------------------------------------------------------
void SpriteDrawingContext::DrawString(Matrix3x2 const& transform, Color const& color,
	FontWeight fontWeight, FontSize fontSize, std::string const& text)
{
	{
		///@todo badcode
		spriteBatch.End();
		spriteBatch.Begin(SpriteSortMode::BackToFront);
	}
	{
		Vector2 fontWeights {0.457f, 0.620f};
	
		switch (fontWeight) {
		case FontWeight::Light:
			fontWeights = {0.440f, 0.800f};
			break;
		case FontWeight::Normal:
			fontWeights = {0.457f, 0.620f};
			break;
		case FontWeight::Bold:
			fontWeights = {0.339f, 0.457f};
			break;
		}
		
		float fontScale = 0.48f;
		
		switch (fontSize) {
		case FontSize::Small:
			fontScale = 0.43f;
			break;
		case FontSize::Medium:
			fontScale = 0.48f;
			break;
		case FontSize::Large:
			fontScale = 0.73f;
			fontWeights *= 1.09f;
			break;
		}

		constantBuffers->Find("DistanceFieldConstants")->SetValue(fontWeights);
	
		///@todo Not implemented
		auto position = Vector2::Transform(Vector2::Zero, transform);// badcode
	
		spriteFont.Begin(Matrix4x4::Identity);
		spriteFont.Draw(spriteFontBatch, text, position, color,
			0.0f, {0.0f, 0.0f}, fontScale, 0.0f);
		spriteFont.End();
	}
}

}// namespace UI
}// namespace Pomdog
