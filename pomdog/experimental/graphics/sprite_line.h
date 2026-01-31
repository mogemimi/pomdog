// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Color;
class Rectangle;
class SpriteBatch;
class Vector2;
} // namespace pomdog

namespace pomdog {

class POMDOG_EXPORT SpriteLine final {
public:
    void draw(
        SpriteBatch& spriteBatch,
        const std::shared_ptr<gpu::Texture2D>& texture,
        const Rectangle& startRect,
        const Rectangle& middleRect,
        const Rectangle& endRect,
        const Vector2& point1,
        const Vector2& point2,
        float lineThickness,
        const Color& color);
};

} // namespace pomdog
