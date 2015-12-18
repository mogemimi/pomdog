// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "SpriteBatch.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cmath>

namespace Pomdog {

class SpriteLine {
public:
    std::shared_ptr<Texture2D> Texture;
    Rectangle StartRectangle;
    Rectangle MiddleRectangle;
    Rectangle EndRectangle;
    Vector2 HalfCircleSize;
    float InverseThickness;

public:
    void Draw(SpriteBatch & spriteBatch, Vector2 const& point1, Vector2 const& point2,
        float lineThickness, Color const& color, float layerDepth);

    template <class BatchRenderer>
    void Draw(BatchRenderer & spriteBatch, Vector2 const& point1, Vector2 const& point2,
        float lineThickness, Color const& color, float layerDepth)
    {
        auto lineLength = Vector2::Distance(point2, point1);

        POMDOG_ASSERT(InverseThickness > 0);
        auto thicknessScale = lineThickness / InverseThickness;

        auto tangent = point2 - point1;
        auto rotation = std::atan2(tangent.Y, tangent.X);

        POMDOG_ASSERT(Texture);

        spriteBatch.Draw(Texture, Matrix3x2::Identity, point1, MiddleRectangle,
            color, rotation, {0.0f, 0.5f}, Vector2{lineLength, thicknessScale}, layerDepth);
        spriteBatch.Draw(Texture, Matrix3x2::Identity, point1, StartRectangle,
            color, rotation, {1.0f, 0.5f}, thicknessScale, layerDepth);
        spriteBatch.Draw(Texture, Matrix3x2::Identity, point2, EndRectangle,
            color, rotation, {0.0f, 0.5f}, thicknessScale, layerDepth);
    }
};

} // namespace Pomdog
