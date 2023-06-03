// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/math/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

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
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        AssetManager& assets);

    SpriteBatch(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        std::optional<gpu::BlendDescriptor>&& blendDesc,
        std::optional<gpu::RasterizerDescriptor>&& rasterizerDesc,
        std::optional<gpu::SamplerDescriptor>&& samplerDesc,
        std::optional<PixelFormat>&& renderTargetViewFormat,
        std::optional<PixelFormat>&& depthStencilViewFormat,
        SpriteBatchPixelShaderMode pixelShaderMode,
        AssetManager& assets);

    SpriteBatch() = delete;
    SpriteBatch(const SpriteBatch&) = delete;
    SpriteBatch(SpriteBatch&&) = default;

    SpriteBatch& operator=(const SpriteBatch&) = delete;
    SpriteBatch& operator=(SpriteBatch&&) = default;

    ~SpriteBatch();

    void begin(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const Matrix4x4& transformMatrix);

    void begin(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const Matrix4x4& transformMatrix,
        const SpriteBatchDistanceFieldParameters& distanceFieldParameters);

    void draw(
        const std::shared_ptr<gpu::Texture2D>& texture,
        const Rectangle& sourceRect,
        const Color& color);

    void draw(
        const std::shared_ptr<gpu::Texture2D>& texture,
        const Vector2& position,
        const Color& color);

    void draw(
        const std::shared_ptr<gpu::Texture2D>& texture,
        const Vector2& position,
        const Rectangle& sourceRect,
        const Color& color);

    void draw(
        const std::shared_ptr<gpu::Texture2D>& texture,
        const Vector2& position,
        const Rectangle& sourceRect,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        float scale);

    void draw(
        const std::shared_ptr<gpu::Texture2D>& texture,
        const Vector2& position,
        const Rectangle& sourceRect,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        const Vector2& scale);

    void draw(
        const std::shared_ptr<gpu::Texture2D>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        float scale);

    void draw(
        const std::shared_ptr<gpu::Texture2D>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        const Vector2& scale);

    void draw(
        const std::shared_ptr<gpu::RenderTarget2D>& texture,
        const Rectangle& sourceRect,
        const Color& color);

    void draw(
        const std::shared_ptr<gpu::RenderTarget2D>& texture,
        const Vector2& position,
        const Color& color);

    void draw(
        const std::shared_ptr<gpu::RenderTarget2D>& texture,
        const Vector2& position,
        const Rectangle& sourceRect,
        const Color& color);

    void draw(
        const std::shared_ptr<gpu::RenderTarget2D>& texture,
        const Vector2& position,
        const Rectangle& sourceRect,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        float scale);

    void draw(
        const std::shared_ptr<gpu::RenderTarget2D>& texture,
        const Vector2& position,
        const Rectangle& sourceRect,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        const Vector2& scale);

    void draw(
        const std::shared_ptr<gpu::RenderTarget2D>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        float scale);

    void draw(
        const std::shared_ptr<gpu::RenderTarget2D>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        const Vector2& scale);

    void flush();

    void end();

    [[nodiscard]] int getDrawCallCount() const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace pomdog
