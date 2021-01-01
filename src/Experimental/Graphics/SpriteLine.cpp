// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Graphics/SpriteLine.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/Graphics/SpriteBatch.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Radian.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Utility/Assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

void SpriteLine::Draw(
    SpriteBatch& spriteBatch,
    const std::shared_ptr<Texture2D>& texture,
    const Rectangle& startRect,
    const Rectangle& middleRect,
    const Rectangle& endRect,
    const Vector2& point1,
    const Vector2& point2,
    float lineThickness,
    const Color& color)
{
    POMDOG_ASSERT(texture);

    if ((middleRect.Width <= 0) || (middleRect.Height <= 0)) {
        return;
    }
    POMDOG_ASSERT(middleRect.Width > 0);
    POMDOG_ASSERT(middleRect.Height > 0);

    auto lineLength = Vector2::Distance(point2, point1);
    auto tangent = point2 - point1;
    auto rotation = std::atan2(tangent.Y, tangent.X);

    spriteBatch.Draw(texture, point1, middleRect,
        color, rotation, {0.0f, 0.5f}, Vector2{lineLength / middleRect.Width, lineThickness});
    spriteBatch.Draw(texture, point1, startRect,
        color, rotation, {1.0f, 0.5f}, lineThickness);
    spriteBatch.Draw(texture, point2, endRect,
        color, rotation, {0.0f, 0.5f}, lineThickness);
}

} // namespace Pomdog
