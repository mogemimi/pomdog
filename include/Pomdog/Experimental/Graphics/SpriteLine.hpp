// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <memory>

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
