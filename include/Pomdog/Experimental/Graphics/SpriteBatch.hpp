// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

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

enum class SpriteBatchPixelShaderMode : std::uint8_t {
    /// Default pixel shader
    Default,

    /// Distance field pixel shader
    DistanceField,
};

struct SpriteBatchDistanceFieldParameters final {
    // NOTE:
    // Smoothing = 1.0/3.0; // 12pt
    // Smoothing = 1.0/5.0; // 32pt
    // Smoothing = 1.0/16.0; // 72pt
    float Smoothing;

    // NOTE:
    // Weight = 0.9; // lighter (300)
    // Weight = 0.54; // normal (400)
    // Weight = 0.4; // bold (700)
    float Weight;
};

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
        AssetManager& assets);

    SpriteBatch(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        std::optional<BlendDescription>&& blendDesc,
        std::optional<SamplerDescription>&& samplerDesc,
        std::optional<SurfaceFormat>&& renderTargetViewFormat,
        std::optional<DepthFormat>&& depthStencilViewFormat,
        SpriteBatchPixelShaderMode pixelShaderMode,
        AssetManager& assets);

    SpriteBatch() = delete;
    SpriteBatch(const SpriteBatch&) = delete;
    SpriteBatch(SpriteBatch&&) = default;

    SpriteBatch& operator=(const SpriteBatch&) = delete;
    SpriteBatch& operator=(SpriteBatch&&) = default;

    ~SpriteBatch();

    void Begin(
        const std::shared_ptr<GraphicsCommandList>& commandList,
        const Matrix4x4& transformMatrix);

    void Begin(
        const std::shared_ptr<GraphicsCommandList>& commandList,
        const Matrix4x4& transformMatrix,
        const SpriteBatchDistanceFieldParameters& distanceFieldParameters);

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

    void Draw(
        const std::shared_ptr<RenderTarget2D>& texture,
        const Rectangle& sourceRect,
        const Color& color);

    void Draw(
        const std::shared_ptr<RenderTarget2D>& texture,
        const Vector2& position,
        const Color& color);

    void Draw(
        const std::shared_ptr<RenderTarget2D>& texture,
        const Vector2& position,
        const Rectangle& sourceRect,
        const Color& color);

    void Draw(
        const std::shared_ptr<RenderTarget2D>& texture,
        const Vector2& position,
        const Rectangle& sourceRect,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        float scale);

    void Draw(
        const std::shared_ptr<RenderTarget2D>& texture,
        const Vector2& position,
        const Rectangle& sourceRect,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        const Vector2& scale);

    void Draw(
        const std::shared_ptr<RenderTarget2D>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        float scale);

    void Draw(
        const std::shared_ptr<RenderTarget2D>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        const Vector2& scale);

    void Flush();

    void End();

    int GetDrawCallCount() const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
