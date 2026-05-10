// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Color;
class Rect2D;
class SpriteBatch;
class Vector2;
} // namespace pomdog

namespace pomdog::gpu {
class Texture2D;
} // namespace pomdog::gpu

namespace pomdog {

class POMDOG_EXPORT SpriteLine final {
public:
    void draw(
        SpriteBatch& spriteBatch,
        const std::shared_ptr<gpu::Texture2D>& texture,
        const Rect2D& startRect,
        const Rect2D& middleRect,
        const Rect2D& endRect,
        const Vector2& point1,
        const Vector2& point2,
        f32 lineThickness,
        const Color& color);
};

} // namespace pomdog
