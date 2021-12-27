// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/graphics/primitive_batch.hpp"
#include "pomdog/experimental/graphics/sprite_batch.hpp"
#include "pomdog/experimental/graphics/sprite_font.hpp"
#include "pomdog/experimental/graphics/truetype_font.hpp"
#include "pomdog/experimental/gui/font_size.hpp"
#include "pomdog/experimental/gui/font_weight.hpp"
#include "pomdog/experimental/texture_packer/texture_atlas.hpp"
#include "pomdog/math/color.hpp"
#include "pomdog/math/matrix3x2.hpp"
#include "pomdog/math/matrix4x4.hpp"
#include "pomdog/math/radian.hpp"
#include "pomdog/math/rectangle.hpp"
#include "pomdog/math/vector2.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class RenderCommand;
class SpriteFont;
} // namespace pomdog

namespace pomdog::gui {

struct ColorScheme final {
    Color SliderTextColor = Color::White();
    Color SliderFillColorBase = {27, 141, 206, 255};
    Color SliderFillColorHovered = {56, 150, 225, 255};
    Color SliderFillColorDisabled = {150, 161, 167, 255};
    Color SliderTrackColorBase = {110, 108, 109, 255};
    Color SliderTrackColorHovered = {121, 120, 120, 255};
    Color SliderThumbColorBase = Color::Black();
    Color SliderThumbColorFocused = {229, 20, 0, 255};

    Color TextFieldTextColor = Color::White();
    Color TextFieldRectColorBase = Color{92, 90, 91, 255};
    Color TextFieldBorderColorBase = Color{104, 100, 101, 255};
    Color TextFieldBorderColorFocus = Color{31, 115, 172, 255};
    Color TextFieldBorderColorError = Color{160, 24, 0, 255};

    Color ScrollBarSliderColorBase = Color{108, 106, 107, 180};
    Color ScrollBarSliderColorHovered = Color{131, 129, 128, 200};
    Color ScrollBarTrackColor = Color{62, 60, 61, 255};
    Color ScrollBarBorderColor = Color{95, 92, 91, 255};

    Color CheckBoxCheckMarkColorBase = Color::White();
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

class DrawingContext final {
public:
    DrawingContext(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        AssetManager& assets);

    Point2D GetCurrentTransform() const;

    void PushTransform(const Point2D& position);

    void PopTransform();

    void Reset(int viewportWidth, int viewportHeight);

    void PushScissorRect(const Rectangle& scissorRect);

    void PopScissorRect();

    void BeginDraw(
        const std::shared_ptr<GraphicsCommandList>& commandList,
        const Matrix4x4& transformMatrix);

    void EndDraw();

    std::shared_ptr<SpriteFont>
    GetFont(FontWeight fontWeight, FontSize fontSize) const;

    PrimitiveBatch* GetPrimitiveBatch();
    SpriteBatch* GetSpriteBatch();

    const ColorScheme* GetColorScheme() const;

    void DrawIcon(
        const std::string& name,
        const Vector2& position,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        float scale);

private:
    std::unordered_map<std::uint32_t, std::shared_ptr<SpriteFont>> spriteFonts;

    TexturePacker::TextureAtlas iconTextureAtlas;
    std::shared_ptr<Texture2D> iconTexture;

    ColorScheme colorScheme;

    std::shared_ptr<GraphicsCommandList> commandList;
    std::shared_ptr<SpriteBatch> spriteBatch;
    std::shared_ptr<PrimitiveBatch> primitiveBatch;
    std::vector<Point2D> matrixStack;
    std::vector<Rectangle> scissorRects;
    int viewportWidth;
    int viewportHeight;
};

} // namespace pomdog::gui
