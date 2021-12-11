// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/sprite_line.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/graphics/sprite_batch.hpp"
#include "pomdog/math/color.hpp"
#include "pomdog/math/radian.hpp"
#include "pomdog/math/rectangle.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

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

} // namespace pomdog