// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class CommandList;
class GraphicsDevice;
class RenderTarget2D;
class Texture2D;
struct BlendDesc;
struct RasterizerDesc;
struct SamplerDesc;
} // namespace pomdog::gpu

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog {
struct TextureRegion;
class Color;
class Error;
class Matrix4x4;
class Rect2D;
class Vector2;
template <typename T>
class Radian;
} // namespace pomdog

namespace pomdog::gpu {
enum class PixelFormat : u8;
} // namespace pomdog::gpu

namespace pomdog {

enum class SpriteBatchPixelShaderMode : u8 {
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
    f32 Smoothing;

    // NOTE:
    // Weight = 0.9; // lighter (300)
    // Weight = 0.54; // normal (400)
    // Weight = 0.4; // bold (700)
    f32 Weight;
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

/// Batches and renders 2D sprites efficiently using instanced rendering.
class POMDOG_EXPORT SpriteBatch final {
public:
    SpriteBatch();

    SpriteBatch(const SpriteBatch&) = delete;
    SpriteBatch(SpriteBatch&&) = default;

    SpriteBatch& operator=(const SpriteBatch&) = delete;
    SpriteBatch& operator=(SpriteBatch&&) = default;

    ~SpriteBatch();

    /// Initializes the sprite batch by loading shaders and creating GPU resources.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice);

    /// Initializes the sprite batch with custom blend, rasterizer, sampler, and format settings.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        std::optional<gpu::BlendDesc>&& blendDesc,
        std::optional<gpu::RasterizerDesc>&& rasterizerDesc,
        std::optional<gpu::SamplerDesc>&& samplerDesc,
        std::optional<gpu::PixelFormat>&& renderTargetViewFormat,
        std::optional<gpu::PixelFormat>&& depthStencilViewFormat,
        SpriteBatchPixelShaderMode pixelShaderMode);

    void begin(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const Matrix4x4& transformMatrix);

    void begin(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const Matrix4x4& transformMatrix,
        const SpriteBatchDistanceFieldParameters& distanceFieldParameters);

    void draw(
        const std::shared_ptr<gpu::Texture2D>& texture,
        const Rect2D& sourceRect,
        const Color& color);

    void draw(
        const std::shared_ptr<gpu::Texture2D>& texture,
        const Vector2& position,
        const Color& color);

    void draw(
        const std::shared_ptr<gpu::Texture2D>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const Color& color);

    void draw(
        const std::shared_ptr<gpu::Texture2D>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        f32 scale);

    void draw(
        const std::shared_ptr<gpu::Texture2D>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        const Vector2& scale);

    void draw(
        const std::shared_ptr<gpu::Texture2D>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        f32 scale);

    void draw(
        const std::shared_ptr<gpu::Texture2D>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        const Vector2& scale);

    void draw(
        const std::shared_ptr<gpu::RenderTarget2D>& texture,
        const Rect2D& sourceRect,
        const Color& color);

    void draw(
        const std::shared_ptr<gpu::RenderTarget2D>& texture,
        const Vector2& position,
        const Color& color);

    void draw(
        const std::shared_ptr<gpu::RenderTarget2D>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const Color& color);

    void draw(
        const std::shared_ptr<gpu::RenderTarget2D>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        f32 scale);

    void draw(
        const std::shared_ptr<gpu::RenderTarget2D>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        const Vector2& scale);

    void draw(
        const std::shared_ptr<gpu::RenderTarget2D>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        f32 scale);

    void draw(
        const std::shared_ptr<gpu::RenderTarget2D>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        const Vector2& scale);

    void flush();

    void end();

    [[nodiscard]] u32
    getDrawCallCount() const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace pomdog
