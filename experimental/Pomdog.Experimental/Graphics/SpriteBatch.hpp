// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SPRITEBATCH_B4DEBA3B_HPP
#define POMDOG_SPRITEBATCH_B4DEBA3B_HPP

#include "SpriteSortMode.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/ConstantBufferBinding.hpp"
#include "Pomdog/Graphics/EffectPass.hpp"
#include "Pomdog/Graphics/GraphicsContext.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Math/Radian.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <memory>

namespace Pomdog {

class SpriteBatch {
public:
    SpriteBatch(std::shared_ptr<GraphicsContext> const& graphicsContext,
        std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        AssetManager & assets);

    SpriteBatch(std::shared_ptr<GraphicsContext> const& graphicsContext,
        std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        std::shared_ptr<EffectPass> const& effectPass,
        std::shared_ptr<ConstantBufferBinding> const& constantBuffers);

    ~SpriteBatch();

    void Begin(SpriteSortMode sortMode);

    void Begin(SpriteSortMode sortMode, Matrix4x4 const& transformMatrix);

    void Draw(std::shared_ptr<Texture2D> const& texture,
        Rectangle const& sourceRect, Color const& color);

    void Draw(std::shared_ptr<Texture2D> const& texture,
        Vector2 const& position, Color const& color);

    void Draw(std::shared_ptr<Texture2D> const& texture,
        Vector2 const& position, Rectangle const& sourceRect, Color const& color);

    void Draw(std::shared_ptr<Texture2D> const& texture,
        Vector2 const& position, Rectangle const& sourceRect, Color const& color,
        Radian<float> const& rotation, Vector2 const& originPivot, float scale, float layerDepth);

    void Draw(std::shared_ptr<Texture2D> const& texture,
        Vector2 const& position, Rectangle const& sourceRect, Color const& color,
        Radian<float> const& rotation, Vector2 const& originPivot, Vector2 const& scale, float layerDepth);

    void End();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

}// namespace Pomdog

#endif // POMDOG_SPRITEBATCH_B4DEBA3B_HPP
