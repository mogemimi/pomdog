// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "SpriteLine.hpp"

namespace Pomdog {

void SpriteLine::Draw(SpriteBatch & spriteBatch, Vector2 const& point1, Vector2 const& point2,
		float lineThickness, Color const& color, float layerDepth)
{
	auto lineLength = Vector2::Distance(point2, point1);

	POMDOG_ASSERT(InverseThickness > 0);
	auto thicknessScale = lineThickness / InverseThickness;

	auto tangent = point2 - point1;
	auto rotation = std::atan2(tangent.Y, tangent.X);

	POMDOG_ASSERT(Texture);

	spriteBatch.Draw(Texture, point1, MiddleRectangle,
		color, rotation, {0.0f, 0.5f}, Vector2{lineLength, thicknessScale}, layerDepth);
	spriteBatch.Draw(Texture, point1, StartRectangle,
		color, rotation, {1.0f, 0.5f}, thicknessScale, layerDepth);
	spriteBatch.Draw(Texture, point2, EndRectangle,
		color, rotation, {0.0f, 0.5f}, thicknessScale, layerDepth);
}

}// namespace Pomdog
