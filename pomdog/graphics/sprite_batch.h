// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/color.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class CommandList;
class GraphicsDevice;
class Texture;
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

/// Specifies which pixel shader to use for sprite rendering.
enum class SpriteBatchPixelShaderMode : u8 {
    /// Default sprite pixel shader.
    Sprite,

    /// Solid-fill pixel shader.
    SolidFill,

    /// Pixel shader for rendering waterline effects.
    WaterLine,

    /// Distance field pixel shader for R8-formatted font texture.
    DistanceField,

    /// Distance field pixel shader for R8-formatted font texture with outline.
    DistanceFieldWithOutline,
};

/// Sorting mode for sprites in a batch.
enum class SpriteSortMode : u8 {
    /// Sorts sprites by texture to minimize texture switching.
    Texture,

    /// Sorts sprites back to front, ensuring proper rendering of transparent objects.
    BackToFront,

    /// Sorts sprites front to back, which can improve performance by allowing depth-based optimizations.
    FrontToBack,
};

/// Optimization hint for SpriteBatch creation.
enum class SpriteBatchOptimizationKind : u8 {
    /// No specific optimization. Allows multiple textures and dynamic sorting.
    NotSpecified,

    /// Assumes pre-sorted sprites using a single texture.
    SortedSingleTexture,
};

/// Parameters for drawing a sprite with full control over appearance.
struct POMDOG_EXPORT SpriteBatchDrawParameters final {
    /// The color to be applied to the sprite.
    Color color = Color::createWhite();

    /// Rotation of the sprite in radians.
    Radian<f32> rotation = Radian<f32>{0.0f};

    /// Origin pivot point for scaling and rotation.
    ///
    /// The bottom-left corner is (0, 0), the top-right corner is (1, 1),
    /// and the center is (0.5, 0.5).
    Vector2 originPivot = Vector2{0.0f, 0.0f};

    /// Scaling factor for the sprite.
    Vector2 scale = Vector2{1.0f, 1.0f};

    /// Depth of the sprite, used for layering.
    f32 layerDepth = 0.0f;
};

/// Provides parameters for drawing a sprite with the solid-fill effect.
struct POMDOG_EXPORT SpriteBatchDrawSolidFill final {
    /// Specifies the color to apply to the sprite.
    Color color = Color::createWhite();

    /// Controls blending between textured and solid-fill colors (0.0 to 1.0).
    f32 blendFactor = 0.0f;

    /// Specifies the rotation of the sprite in radians.
    Radian<f32> rotation = Radian<f32>{0.0f};

    /// Specifies the origin pivot point for scaling and rotation.
    ///
    /// The bottom-left corner is (0, 0), the top-right corner is (1, 1),
    /// and the center is (0.5, 0.5).
    Vector2 originPivot = Vector2{0.0f, 0.0f};

    /// Specifies the scaling factor for the sprite.
    Vector2 scale = Vector2{1.0f, 1.0f};

    /// Specifies the depth of the sprite used for layering.
    f32 layerDepth = 0.0f;
};

/// Parameters for drawing a distance field sprite with SDF font rendering.
struct POMDOG_EXPORT SpriteBatchDrawDistanceField final {
    /// The glyph color.
    Color color = Color::createWhite();

    /// The color used for font outlines.
    Color outlineColor = Color::createTransparentBlack();

    /// Controls the amount of SDF font smoothing (0.0 to 1.0).
    ///
    /// Values near 0.0 produce sharper edges. Values near 1.0 produce softer,
    /// blurrier edges.
    ///
    /// Keep `fontSmoothing + fontWeight` at or below 1.0 when rendering
    /// SpriteFont glyphs so zero-valued atlas padding remains transparent.
    ///
    /// The default suits an effective scale of 1.0 without an outline. For
    /// scale-aware values, see `computeSpriteFontSDFParameters()` helper.
    f32 fontSmoothing = 0.140f;

    /// Controls the SDF font weight from thin (0.0) to thick (1.0).
    ///
    /// Keep `fontSmoothing + fontWeight` at or below 1.0 when rendering
    /// SpriteFont glyphs so zero-valued atlas padding remains transparent.
    ///
    /// The default suits an effective scale of 1.0 without an outline. For
    /// scale-aware values, see `computeSpriteFontSDFParameters()` helper.
    f32 fontWeight = 0.560f;

    /// Controls the SDF outline threshold (0.0 to 1.0).
    ///
    /// Values near 0.0 produce a thinner outline. Values near 1.0 produce a
    /// thicker outline.
    f32 outlineWeight = 0.440f;

    /// Rotation of the sprite in radians.
    Radian<f32> rotation = Radian<f32>{0.0f};

    /// Origin pivot point for scaling and rotation.
    ///
    /// The bottom-left corner is (0, 0), the top-right corner is (1, 1),
    /// and the center is (0.5, 0.5).
    Vector2 originPivot = Vector2{0.0f, 0.0f};

    /// Scaling factor for the sprite.
    Vector2 scale = Vector2{1.0f, 1.0f};

    /// Depth of the sprite, used for layering.
    f32 layerDepth = 0.0f;
};

/// Parameters for drawing a sprite with the waterline effect.
struct POMDOG_EXPORT SpriteBatchDrawWaterline final {
    /// Primary color for the area above the water line.
    Color color = Color::createWhite();

    /// Secondary color for the area below the water line.
    Color color1 = Color::createWhite();

    /// Blend factor for solid-fill mixing above the water line (0.0 to 1.0).
    f32 blendFactor = 0.0f;

    /// Blend factor for solid-fill mixing below the water line (0.0 to 1.0).
    f32 blendFactorForWaterLine = 0.0f;

    /// Y position of the water line in world space.
    f32 waterLineYPosition = -30000.0f;

    /// Rotation of the sprite in radians.
    Radian<f32> rotation = Radian<f32>{0.0f};

    /// Origin pivot point for scaling and rotation.
    ///
    /// The bottom-left corner is (0, 0), the top-right corner is (1, 1),
    /// and the center is (0.5, 0.5).
    Vector2 originPivot = Vector2{0.0f, 0.0f};

    /// Scaling factor for the sprite.
    Vector2 scale = Vector2{1.0f, 1.0f};

    /// Depth of the sprite, used for layering.
    f32 layerDepth = 0.0f;
};

// NOTE: SpriteBatch uses the Cartesian coordinate system in which sprite is drawn.
// Also the `originPivot` represents a anchor point of sprite.
// If you want to align the bottom-left corner, the pivot is set to (0, 0).
// If you want to align the top-right corner, the pivot is set to (1, 1).
//
//                     width
//              ◄───────────────────►
//           (0, 1)              (1, 1)
//        ▲     ┌───────────────────┐
//        │     │                   │
//        │     │                   │
// height │     │         *         │
//        │     │     (0.5, 0.5)    │
//        │     │                   │
//        ▼     └───────────────────┘
//           (0, 0)              (1, 0)

/// Holds the pipeline state for sprite rendering.
class POMDOG_EXPORT SpritePipeline {
public:
    virtual ~SpritePipeline();
};

/// Batches and renders 2D sprites efficiently using instanced rendering.
///
/// Usage pattern:
/// ```cpp
/// spriteBatch->reset();
/// spriteBatch->setTransform(projectionMatrix);
/// spriteBatch->draw(texture, position, sourceRect, color, ...);
/// spriteBatch->sort(SpriteSortMode::Texture);
/// spriteBatch->flush(commandList, spritePipeline);
/// ```
class POMDOG_EXPORT SpriteBatch {
public:
    virtual ~SpriteBatch();

    /// Resets the batch state, clearing any previously batched sprites.
    virtual void
    reset() = 0;

    /// Sets the view-projection matrix applied to every sprite in the batch.
    virtual void
    setTransform(const Matrix4x4& transformMatrix) = 0;

    /// Draws sourceRect of texture at position, tinted by color.
    virtual void
    draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const Color& color) = 0;

    /// Draws sourceRect of texture with a rotation, origin pivot, and uniform scale.
    virtual void
    draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        f32 scale) = 0;

    /// Draws sourceRect of texture with a rotation, origin pivot, and non-uniform scale.
    virtual void
    draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        const Vector2& scale) = 0;

    /// Draws textureRegion of texture with a rotation, origin pivot, and uniform scale.
    virtual void
    draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        f32 scale) = 0;

    /// Draws textureRegion of texture with a rotation, origin pivot, and non-uniform scale.
    virtual void
    draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        const Vector2& scale) = 0;

    /// Draws sourceRect of texture using extended draw parameters.
    virtual void
    draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const SpriteBatchDrawParameters& params) = 0;

    /// Draws textureRegion of texture using extended draw parameters.
    virtual void
    draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const SpriteBatchDrawParameters& params) = 0;

    /// Draws sourceRect of texture using solid-fill parameters.
    virtual void
    draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const SpriteBatchDrawSolidFill& params) = 0;

    /// Draws textureRegion of texture using solid-fill parameters.
    virtual void
    draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const SpriteBatchDrawSolidFill& params) = 0;

    /// Draws sourceRect of texture using distance field parameters.
    virtual void
    draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const SpriteBatchDrawDistanceField& params) = 0;

    /// Draws textureRegion of texture using distance field parameters.
    virtual void
    draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const SpriteBatchDrawDistanceField& params) = 0;

    /// Draws sourceRect of texture using waterline effect parameters.
    virtual void
    draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const SpriteBatchDrawWaterline& params) = 0;

    /// Draws textureRegion of texture using waterline effect parameters.
    virtual void
    draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const SpriteBatchDrawWaterline& params) = 0;

    /// Sorts the batched sprites using the given sort mode.
    virtual void
    sort(SpriteSortMode sortMode) = 0;

    /// Records the batched sprites as draw commands into commandList.
    ///
    /// Only records GPU draw commands using spritePipeline; the vertex data
    /// upload is deferred to submit(). Multiple flush() calls with different
    /// pipelines can share the same batch.
    virtual void
    flush(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const std::shared_ptr<SpritePipeline>& spritePipeline) = 0;

    /// Uploads the batched sprite instance data from CPU to GPU.
    ///
    /// Call once per frame after all flush() calls. When the instance count
    /// exceeds the current buffer capacity, the buffer is resized to fit.
    virtual void
    submit(const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice) = 0;

    /// Returns the number of draw calls issued during the last flush.
    [[nodiscard]] virtual u32
    getDrawCallCount() const noexcept = 0;
};

/// Creates a SpritePipeline for the given pixel shader mode.
///
/// fs loads the shader assets. The optional descriptors and pixel formats
/// override the defaults for the blend, rasterizer, and sampler state and for
/// the render target and depth stencil formats when set.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<SpritePipeline>, std::unique_ptr<Error>>
createSpritePipeline(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<gpu::BlendDesc>&& blendDesc,
    std::optional<gpu::RasterizerDesc>&& rasterizerDesc,
    std::optional<gpu::SamplerDesc>&& samplerDesc,
    std::optional<gpu::PixelFormat>&& renderTargetViewFormat,
    std::optional<gpu::PixelFormat>&& depthStencilViewFormat,
    SpriteBatchPixelShaderMode pixelShaderMode) noexcept;

/// Creates a SpriteBatch.
///
/// batchSize is the initial instance capacity, which defaults to 2048 and is
/// clamped to the range [64, 65536]. The buffer grows on demand in submit().
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<SpriteBatch>, std::unique_ptr<Error>>
createSpriteBatch(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<u32> batchSize = std::nullopt) noexcept;

/// Creates a SpriteBatch with an optimization hint.
///
/// optimization lets the batch assume a usage pattern, such as pre-sorted
/// sprites that share a single texture. batchSize defaults to 2048 and is
/// clamped to [64, 65536].
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<SpriteBatch>, std::unique_ptr<Error>>
createSpriteBatch(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<u32> batchSize,
    SpriteBatchOptimizationKind optimization) noexcept;

} // namespace pomdog
