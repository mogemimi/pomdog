// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/math/forward_declarations.hpp"
#include "pomdog/math/vector2.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class SpriteBatch;

class POMDOG_EXPORT SpriteLine final {
public:
    void Draw(
        SpriteBatch& spriteBatch,
        const std::shared_ptr<Texture2D>& texture,
        const Rectangle& startRect,
        const Rectangle& middleRect,
        const Rectangle& endRect,
        const Vector2& point1,
        const Vector2& point2,
        float lineThickness,
        const Color& color);
};

} // namespace pomdog
