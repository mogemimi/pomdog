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

    /// Secondary color for advanced effects (e.g., gradients).
    Color color1 = Color::createWhite();

    /// Blend factor for color blending (0.0 to 1.0).
    f32 blendFactor = 0.0f;

    /// Blend factor for color blending (0.0 to 1.0).
    f32 blendFactorForWaterLine = 0.0f;

    /// Y position of the water line for water effects.
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

    /// Sets the transformation matrix to be applied to all sprites in the batch.
    ///
    /// @param transformMatrix The view-projection matrix.
    virtual void
    setTransform(const Matrix4x4& transformMatrix) = 0;

    /// Sets the transformation matrix and distance field font parameters.
    ///
    /// @param transformMatrix The view-projection matrix.
    /// @param fontSmoothing Controls the amount of font smoothing (0.0 to 1.0).
    /// @param fontWeight Controls the font's weight (0.0 to 1.0).
    /// @param outlineColor The color used for font outlines.
    /// @param outlineWeight The thickness of the font outline (0.0 to 1.0).
    virtual void
    setTransform(
        const Matrix4x4& transformMatrix,
        f32 fontSmoothing,
        f32 fontWeight,
        const Color& outlineColor,
        f32 outlineWeight) = 0;

    /// Draws a sprite at the specified position with the given source rectangle and color.
    ///
    /// @param texture The texture to draw.
    /// @param position The position of the sprite.
    /// @param sourceRect The source rectangle within the texture.
    /// @param color The color tint to apply.
    virtual void
    draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const Color& color) = 0;

    /// Draws a sprite with rotation, origin pivot, and uniform scale.
    ///
    /// @param texture The texture to draw.
    /// @param position The position of the sprite.
    /// @param sourceRect The source rectangle within the texture.
    /// @param color The color tint to apply.
    /// @param rotation The rotation in radians.
    /// @param originPivot The origin pivot point.
    /// @param scale The uniform scale factor.
    virtual void
    draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        f32 scale) = 0;

    /// Draws a sprite with rotation, origin pivot, and non-uniform scale.
    ///
    /// @param texture The texture to draw.
    /// @param position The position of the sprite.
    /// @param sourceRect The source rectangle within the texture.
    /// @param color The color tint to apply.
    /// @param rotation The rotation in radians.
    /// @param originPivot The origin pivot point.
    /// @param scale The non-uniform scale factor.
    virtual void
    draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        const Vector2& scale) = 0;

    /// Draws a sprite from a texture region with rotation, origin pivot, and uniform scale.
    ///
    /// @param texture The texture to draw.
    /// @param position The position of the sprite.
    /// @param textureRegion The texture region defining the sub-area to draw.
    /// @param color The color tint to apply.
    /// @param rotation The rotation in radians.
    /// @param originPivot The origin pivot point.
    /// @param scale The uniform scale factor.
    virtual void
    draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        f32 scale) = 0;

    /// Draws a sprite from a texture region with rotation, origin pivot, and non-uniform scale.
    ///
    /// @param texture The texture to draw.
    /// @param position The position of the sprite.
    /// @param textureRegion The texture region defining the sub-area to draw.
    /// @param color The color tint to apply.
    /// @param rotation The rotation in radians.
    /// @param originPivot The origin pivot point.
    /// @param scale The non-uniform scale factor.
    virtual void
    draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        const Vector2& scale) = 0;

    /// Draws a sprite using extended draw parameters.
    ///
    /// @param texture The texture to draw.
    /// @param position The position of the sprite.
    /// @param sourceRect The source rectangle within the texture.
    /// @param params Additional parameters controlling sprite appearance.
    virtual void
    draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const Rect2D& sourceRect,
        const SpriteBatchDrawParameters& params) = 0;

    /// Draws a sprite from a texture region using extended draw parameters.
    ///
    /// @param texture The texture to draw.
    /// @param position The position of the sprite.
    /// @param textureRegion The texture region defining the sub-area to draw.
    /// @param params Additional parameters controlling sprite appearance.
    virtual void
    draw(
        const std::shared_ptr<gpu::Texture>& texture,
        const Vector2& position,
        const TextureRegion& textureRegion,
        const SpriteBatchDrawParameters& params) = 0;

    /// Sorts the batched sprites using the specified sort mode.
    ///
    /// @param sortMode The sorting mode (by texture, front-to-back, or back-to-front).
    virtual void
    sort(SpriteSortMode sortMode) = 0;

    /// Flushes the batched sprites, issuing draw commands to the command list.
    ///
    /// This only records GPU draw commands; actual vertex data upload is deferred to submit().
    /// Multiple flush() calls with different pipelines can share the same batch.
    ///
    /// @param commandList The command list to record draw commands into.
    /// @param spritePipeline The sprite pipeline used for rendering.
    virtual void
    flush(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const std::shared_ptr<SpritePipeline>& spritePipeline) = 0;

    /// Uploads batched sprite instance data from CPU to GPU.
    ///
    /// Call once per frame after all flush() calls. If the instance count exceeds
    /// the current buffer capacity, the buffer is resized to fit.
    ///
    /// @param graphicsDevice The graphics device used to create/resize GPU buffers.
    virtual void
    submit(const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice) = 0;

    /// Returns the number of draw calls issued during the last flush.
    [[nodiscard]] virtual u32
    getDrawCallCount() const noexcept = 0;
};

/// Creates a SpritePipeline instance.
///
/// @param fs The file system context for loading shader assets.
/// @param graphicsDevice The graphics device.
/// @param blendDesc Optional blend state descriptor.
/// @param rasterizerDesc Optional rasterizer state descriptor.
/// @param samplerDesc Optional sampler state descriptor.
/// @param renderTargetViewFormat Optional render target pixel format.
/// @param depthStencilViewFormat Optional depth stencil pixel format.
/// @param pixelShaderMode The pixel shader mode to use.
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

/// Creates a SpriteBatch instance.
///
/// @param graphicsDevice The graphics device.
/// @param batchSize Optional initial batch size (default: 2048, range: [64, 65536]).
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<SpriteBatch>, std::unique_ptr<Error>>
createSpriteBatch(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<u32> batchSize = std::nullopt) noexcept;

/// Creates a SpriteBatch instance with optimization settings.
///
/// @param graphicsDevice The graphics device.
/// @param batchSize Optional initial batch size (default: 2048, range: [64, 65536]).
/// @param optimization The optimization hint for the batch.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<SpriteBatch>, std::unique_ptr<Error>>
createSpriteBatch(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::optional<u32> batchSize,
    SpriteBatchOptimizationKind optimization) noexcept;

} // namespace pomdog
