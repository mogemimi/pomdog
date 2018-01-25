// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "SpriteLine.hpp"

namespace Pomdog {

void SpriteLine::Draw(SpriteBatch & spriteBatch, const Vector2& point1, const Vector2& point2,
        float lineThickness, const Color& color, float layerDepth)
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

} // namespace Pomdog
