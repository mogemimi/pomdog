// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/sprite_line.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/math/color.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/math/vector2.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

void SpriteLine::draw(
    SpriteBatch& spriteBatch,
    const std::shared_ptr<gpu::Texture2D>& texture,
    const Rect2D& startRect,
    const Rect2D& middleRect,
    const Rect2D& endRect,
    const Vector2& point1,
    const Vector2& point2,
    f32 lineThickness,
    const Color& color)
{
    POMDOG_ASSERT(texture);

    if ((middleRect.width <= 0) || (middleRect.height <= 0)) {
        return;
    }
    POMDOG_ASSERT(middleRect.width > 0);
    POMDOG_ASSERT(middleRect.height > 0);

    const auto lineLength = math::distance(point2, point1);
    const auto tangent = point2 - point1;
    const auto rotation = std::atan2(tangent.y, tangent.x);

    spriteBatch.draw(
        texture,
        point1,
        middleRect,
        color,
        rotation,
        {0.0f, 0.5f},
        Vector2{lineLength / middleRect.width, lineThickness});

    spriteBatch.draw(
        texture,
        point1,
        startRect,
        color,
        rotation,
        {1.0f, 0.5f},
        lineThickness);

    spriteBatch.draw(
        texture,
        point2,
        endRect,
        color,
        rotation,
        {0.0f, 0.5f},
        lineThickness);
}

} // namespace pomdog
