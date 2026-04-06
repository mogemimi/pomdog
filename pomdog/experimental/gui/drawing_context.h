// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/graphics/primitive_batch.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/experimental/graphics/sprite_font.h"
#include "pomdog/experimental/graphics/truetype_font.h"
#include "pomdog/experimental/gui/font_size.h"
#include "pomdog/experimental/gui/font_weight.h"
#include "pomdog/experimental/texture_packer/texture_atlas.h"
#include "pomdog/math/color.h"
#include "pomdog/math/matrix3x2.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class RenderCommand;
class SpriteFont;
} // namespace pomdog

namespace pomdog::gpu {
class GraphicsDevice;
class Texture2D;
} // namespace pomdog::gpu

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog::gui {

struct ColorScheme final {
    Color SliderTextColor = Color::createWhite();
    Color SliderFillColorBase = {27, 141, 206, 255};
    Color SliderFillColorHovered = {56, 150, 225, 255};
    Color SliderFillColorDisabled = {150, 161, 167, 255};
    Color SliderTrackColorBase = {110, 108, 109, 255};
    Color SliderTrackColorHovered = {121, 120, 120, 255};
    Color SliderThumbColorBase = Color::createBlack();
    Color SliderThumbColorFocused = {229, 20, 0, 255};

    Color TextFieldTextColor = Color::createWhite();
    Color TextFieldRectColorBase = Color{92, 90, 91, 255};
    Color TextFieldBorderColorBase = Color{104, 100, 101, 255};
    Color TextFieldBorderColorFocus = Color{31, 115, 172, 255};
    Color TextFieldBorderColorError = Color{160, 24, 0, 255};

    Color ScrollBarSliderColorBase = Color{108, 106, 107, 180};
    Color ScrollBarSliderColorHovered = Color{131, 129, 128, 200};
    Color ScrollBarTrackColor = Color{62, 60, 61, 255};
    Color ScrollBarBorderColor = Color{95, 92, 91, 255};

    Color CheckBoxCheckMarkColorBase = Color::createWhite();
    Color CheckBoxCheckMarkColorDisabled = Color{137, 137, 137, 255};
    Color CheckBoxRectColorOn = Color{27, 141, 206, 255};
    Color CheckBoxRectColorOff = Color{160, 160, 160, 255};
    Color CheckBoxRectColorDisabled = Color{81, 81, 81, 255};

    Color ContextMenuBackgroundColor = Color{45, 45, 48, 255};
    Color ContextMenuShadowColor = Color{15, 15, 16, 20};
    Color ContextMenuBorderColor = Color{104, 100, 101, 255};
    Color ContextMenuOutlineColor = Color{31, 31, 30, 255};

    Color PushButtonTextColorBase = Color{251, 250, 248, 255};
    Color PushButtonTextColorDisabled = Color{191, 190, 189, 255};
    Color PushButtonRectColorBase = Color{0, 132, 190, 255};
    Color PushButtonRectColorHovered = Color{0, 123, 182, 255};
    Color PushButtonRectColorClick = Color{0, 107, 162, 255};
    Color PushButtonRectColorDisabled = Color{110, 108, 107, 255};

    Color PanelBackgroundColor = Color{45, 45, 48, 225};
    Color PanelOutlineBorderColor = Color{40, 40, 40, 255};
    Color PanelTitleBarColor = Color{106, 106, 106, 255};

    Color ListViewBackgroundColorOdd = Color{40, 40, 40, 255};
    Color ListViewBackgroundColorEven = Color{45, 45, 48, 255};
    Color ListViewItemColorHovered = Color{68, 67, 65, 255};
    Color ListViewItemColorSelected = Color{110, 108, 109, 255};
    Color ListViewItemColorActive = Color{0, 123, 182, 255};

    // Color ButtonPrimaryColor;
    // Color ButtonSecondaryColor;
    // Color ButtonErrorColor;
};

/// Provides rendering primitives for GUI widget drawing.
class DrawingContext final {
public:
    DrawingContext() = default;

    /// Initializes the drawing context by creating batches, loading fonts, and building icon atlas.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<vfs::FileSystemContext>& fs);

    [[nodiscard]] Point2D
    getCurrentTransform() const;

    void
    pushTransform(const Point2D& position);

    void
    popTransform();

    void
    reset(int viewportWidth, int viewportHeight);

    void
    pushScissorRect(const Rect2D& scissorRect);

    void
    popScissorRect();

    void
    beginDraw(
        const std::shared_ptr<gpu::CommandList>& commandList,
        const Matrix4x4& transformMatrix);

    void
    endDraw();

    std::shared_ptr<SpriteFont>
    getFont(FontWeight fontWeight, FontSize fontSize) const;

    [[nodiscard]] PrimitiveBatch*
    getPrimitiveBatch();

    [[nodiscard]] SpriteBatch*
    getSpriteBatch();

    void
    flushPrimitiveBatch();

    void
    flushSpriteBatch();

    [[nodiscard]] const ColorScheme*
    getColorScheme() const;

    void
    drawIcon(
        const std::string& name,
        const Vector2& position,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        float scale);

private:
    std::unordered_map<u32, std::shared_ptr<SpriteFont>> spriteFonts_;

    TexturePacker::TextureAtlas iconTextureAtlas_;
    std::shared_ptr<gpu::Texture2D> iconTexture_;

    ColorScheme colorScheme_;

    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<gpu::CommandList> commandList_;
    std::shared_ptr<SpriteBatch> spriteBatch_;
    std::shared_ptr<SpritePipeline> spritePipeline_;
    std::shared_ptr<SpriteBatch> spriteBatchFont_;
    std::shared_ptr<SpritePipeline> spritePipelineFont_;
    std::shared_ptr<PrimitiveBatch> primitiveBatch_;
    std::shared_ptr<PrimitivePipeline> primitivePipeline_;
    std::vector<Point2D> matrixStack_;
    std::vector<Rect2D> scissorRects_;
    int viewportWidth_;
    int viewportHeight_;
};

} // namespace pomdog::gui
