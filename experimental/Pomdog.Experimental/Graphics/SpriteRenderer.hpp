// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "SpriteSortMode.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Math/Radian.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <memory>

namespace Pomdog {

class SpriteRenderer {
public:
    SpriteRenderer(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        AssetManager & assets);

    ~SpriteRenderer();

    void SetProjectionMatrix(const Matrix4x4& projectionMatrix);

    void Begin(SpriteSortMode sortMode);

    void Begin(SpriteSortMode sortMode, const Matrix4x4& transformMatrix);

    void Draw(const std::shared_ptr<Texture2D>& texture, const Matrix3x2& worldMatrix,
        const Rectangle& sourceRect, const Color& color);

    void Draw(const std::shared_ptr<Texture2D>& texture, const Matrix3x2& worldMatrix,
        const Vector2& position, const Color& color);

    void Draw(const std::shared_ptr<Texture2D>& texture, const Matrix3x2& worldMatrix,
        const Vector2& position, const Rectangle& sourceRect, const Color& color);

    void Draw(const std::shared_ptr<Texture2D>& texture, const Matrix3x2& worldMatrix,
        const Vector2& position, const Rectangle& sourceRect, const Color& color,
        const Radian<float>& rotation, const Vector2& originPivot, float scale, float layerDepth);

    void Draw(const std::shared_ptr<Texture2D>& texture, const Matrix3x2& worldMatrix,
        const Vector2& position, const Rectangle& sourceRect, const Color& color,
        const Radian<float>& rotation, const Vector2& originPivot, const Vector2& scale, float layerDepth);

    void End();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
