// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Graphics/SamplerDescription.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <memory>
#include <optional>

namespace Pomdog {

class AssetManager;
struct TextureRegion;

// NOTE: SpriteBatch uses the Cartesian coordinate system in which sprite is drawn.
// Also the `originPivot` represents a anchor point of sprite.
// If you want to align the bottom-left corner, the pivot is set to (0, 0).
// If you want to align the top-right corner, the pivot is set to (1, 1).
//
//                     width
//              <------------------->
//           (0, 1)              (1, 1)
//        ^     +-------------------+
//        |     |                   |
//        |     |                   |
// height |     |         *         |
//        |     |     (0.5, 0.5)    |
//        |     |                   |
//        v     +-------------------+
//           (0, 0)              (1, 0)

class POMDOG_EXPORT SpriteBatch final {
public:
    SpriteBatch(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        AssetManager & assets);

    SpriteBatch(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        std::optional<BlendDescription>&& blendDesc,
        std::optional<SamplerDescription>&& samplerDesc,
        std::optional<SurfaceFormat>&& renderTargetViewFormat,
        std::optional<DepthFormat>&& depthStencilViewFormat,
        AssetManager & assets);

    SpriteBatch() = delete;
    SpriteBatch(const SpriteBatch&) = delete;
    SpriteBatch(SpriteBatch&&) = default;

    SpriteBatch& operator=(const SpriteBatch&) = delete;
    SpriteBatch& operator=(SpriteBatch&&) = default;

    ~SpriteBatch();

    void Begin(
        const std::shared_ptr<GraphicsCommandList>& commandList,
        const Matrix4x4& transformMatrix);

    void Draw(
        const std::shared_ptr<Texture2D>& texture,
        const Rectangle& sourceRect,
        const Color& color);

    void Draw(
        const std::shared_ptr<Texture2D>& texture,
        const Vector2& position,
        const Color& color);

    void Draw(
        const std::shared_ptr<Texture2D>& texture,
        const Vector2& position,
        const Rectangle& sourceRect,
        const Color& color);

    void Draw(
        const std::shared_ptr<Texture2D>& texture,
        const Vector2& position,
        const Rectangle& sourceRect,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        float scale);

    void Draw(
        const std::shared_ptr<Texture2D>& texture,
        const Vector2& position,
        const Rectangle& sourceRect,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        const Vector2& scale);

    void Draw(
        const std::shared_ptr<Texture2D>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        float scale);

    void Draw(
        const std::shared_ptr<Texture2D>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        const Vector2& scale);

    void End();

    int GetDrawCallCount() const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
