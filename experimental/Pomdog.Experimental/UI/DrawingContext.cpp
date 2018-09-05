// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/UI/DrawingContext.hpp"
#include "Pomdog.Experimental/Rendering/Processors/PrimitivePolygonCommandProcessor.hpp"
#include "Pomdog.Experimental/Rendering/Processors/SpriteBatchCommandProcessor.hpp"

namespace Pomdog {
namespace UI {

DrawingContext::DrawingContext(
    const std::shared_ptr<GraphicsDevice>& graphicsDeviceIn,
    AssetManager & assets)
    : renderer(graphicsDeviceIn)
    , graphicsDevice(graphicsDeviceIn)
    , viewportWidth(1)
    , viewportHeight(1)
{
    using Pomdog::Rendering::SpriteBatchCommandProcessor;
    using Pomdog::Rendering::PrimitivePolygonCommandProcessor;
    renderer.AddProcessor(std::make_unique<SpriteBatchCommandProcessor>(graphicsDevice, assets));
    renderer.AddProcessor(std::make_unique<PrimitivePolygonCommandProcessor>(graphicsDevice, assets));

    fontRegular = std::make_shared<TrueTypeFont>(assets, "fonts/NotoSans/NotoSans-Regular.ttf");
    fontBold = std::make_shared<TrueTypeFont>(assets, "fonts/NotoSans/NotoSans-Bold.ttf");
}

Matrix3x2 DrawingContext::Top() const
{
    POMDOG_ASSERT(!matrixStack.empty());
    return matrixStack.back();
}

void DrawingContext::Push(const Matrix3x2& matrix)
{
    matrixStack.push_back(matrix);
}

void DrawingContext::Pop()
{
    POMDOG_ASSERT(!matrixStack.empty());
    matrixStack.pop_back();
}

std::shared_ptr<GraphicsCommandList> DrawingContext::Render()
{
    return renderer.Render();
}

void DrawingContext::Reset(int viewportWidthIn, int viewportHeightIn)
{
    renderer.Reset();

    viewportWidth = viewportWidthIn;
    viewportHeight = viewportHeightIn;

    auto viewMatrix = Matrix4x4::CreateTranslation(Vector3{
        -viewportWidth * 0.5f,
        -viewportHeight * 0.5f,
        0.0f
    });

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        viewportWidth,
        viewportHeight,
        0.1f,
        100.0f);

    renderer.SetViewMatrix(viewMatrix);
    renderer.SetProjectionMatrix(projectionMatrix);
}

std::shared_ptr<SpriteFont> DrawingContext::GetFont(FontWeight fontWeight, FontSize fontSize)
{
    POMDOG_ASSERT(graphicsDevice);

    std::string fontId;
    switch (fontWeight) {
    case FontWeight::Light:
    case FontWeight::Normal:
        fontId += "N_";
        break;
    case FontWeight::Bold:
        fontId += "B_";
        break;
    }

    switch (fontSize) {
    case FontSize::Small:
        fontId += "S_";
        break;
    case FontSize::Medium:
        fontId += "M_";
        break;
    case FontSize::Large:
        fontId += "L_";
        break;
    }

    auto iter = spriteFonts.find(fontId);
    if (iter != spriteFonts.end()) {
        return iter->second;
    }

    std::shared_ptr<TrueTypeFont> font;
    switch (fontWeight) {
    case FontWeight::Light:
    case FontWeight::Normal:
        font = fontRegular;
        break;
    case FontWeight::Bold:
        font = fontBold;
        break;
    }

    std::int16_t fontPointSize = 26;
    switch (fontSize) {
    case FontSize::Small:
        fontPointSize = 13;
        break;
    case FontSize::Medium:
        fontPointSize = 15;
        break;
    case FontSize::Large:
        fontPointSize = 26;
        break;
    }

    auto spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, fontPointSize, fontPointSize);
    spriteFont->SetDefaultCharacter(U'?');
    spriteFont->MeasureString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,-+*/?&!");
    spriteFonts.emplace(fontId, spriteFont);

    return spriteFont;
}

void DrawingContext::PushCommand(std::reference_wrapper<RenderCommand> && command)
{
    renderer.PushCommand(std::move(command));
}

} // namespace UI
} // namespace Pomdog
