// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Math/ForwardDeclarations.hpp"
#include "Pomdog/Math/Vector2.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

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

} // namespace Pomdog
