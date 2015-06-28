// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SPRITEBATCHRENDERER_FAFB0E53_HPP
#define POMDOG_SPRITEBATCHRENDERER_FAFB0E53_HPP

#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

class SpriteBatchRenderer {
public:
    SpriteBatchRenderer(
        std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        AssetManager & assets);

    ~SpriteBatchRenderer();

    void Begin(
        std::shared_ptr<GraphicsCommandList> const& commandList,
        Matrix4x4 const& transformMatrix);

    void Draw(
        std::shared_ptr<Texture2D> const& texture,
        Matrix3x2 const& worldMatrix,
        Color const& color,
        Vector2 const& originPivot);

    void Draw(
        std::shared_ptr<Texture2D> const& texture,
        Matrix3x2 const& worldMatrix,
        Rectangle const& sourceRect,
        Color const& color,
        Vector2 const& originPivot);

    void Draw(
        std::shared_ptr<Texture2D> const& texture,
        Rectangle const& sourceRect,
        Color const& color);

    void Draw(
        std::shared_ptr<Texture2D> const& texture,
        Vector2 const& position,
        Color const& color);

    void Draw(
        std::shared_ptr<Texture2D> const& texture,
        Vector2 const& position,
        Rectangle const& sourceRect,
        Color const& color);

    void Draw(
        std::shared_ptr<Texture2D> const& texture,
        Vector2 const& position,
        Rectangle const& sourceRect,
        Color const& color,
        Radian<float> const& rotation,
        Vector2 const& originPivot,
        float scale);

    void Draw(
        std::shared_ptr<Texture2D> const& texture,
        Vector2 const& position,
        Rectangle const& sourceRect,
        Color const& color,
        Radian<float> const& rotation,
        Vector2 const& originPivot,
        Vector2 const& scale);

    void End();

    std::uint32_t GetDrawCallCount() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog

#endif // POMDOG_SPRITEBATCHRENDERER_FAFB0E53_HPP
