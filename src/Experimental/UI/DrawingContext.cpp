// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/UI/DrawingContext.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Experimental/Graphics/TrueTypeFontLoader.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Graphics/DepthStencilDescription.hpp"
#include "Pomdog/Graphics/GraphicsCommandList.hpp"
#include "Pomdog/Graphics/RasterizerDescription.hpp"
#include "Pomdog/Graphics/SamplerDescription.hpp"
#include "Pomdog/Logging/Log.hpp"

namespace Pomdog::UI {
namespace {

std::uint32_t MakeFontID(FontWeight fontWeight, FontSize fontSize)
{
    auto fontID = static_cast<std::uint32_t>(fontWeight);
    fontID |= (static_cast<std::uint32_t>(fontSize) << 8);
    return fontID;
}

} // namespace

DrawingContext::DrawingContext(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    AssetManager& assets)
    : viewportWidth(1)
    , viewportHeight(1)
{
    primitiveBatch = std::make_shared<PrimitiveBatch>(
        graphicsDevice,
        std::nullopt,
        RasterizerDescription::CreateCullNone(),
        assets);

    spriteBatch = std::make_shared<SpriteBatch>(
        graphicsDevice,
        BlendDescription::CreateNonPremultiplied(),
        RasterizerDescription::CreateCullNone(),
        SamplerDescription::CreateLinearWrap(),
        std::nullopt,
        std::nullopt,
        SpriteBatchPixelShaderMode::Default,
        assets);

    std::shared_ptr<Error> err;
    std::tie(fontRegular, err) = assets.Load<TrueTypeFont>("Fonts/NotoSans/NotoSans-Medium.ttf");
    if (err != nullptr) {
        Log::Critical("Pomdog", "failed to load font file");
    }

    std::tie(fontBold, err) = assets.Load<TrueTypeFont>("Fonts/NotoSans/NotoSans-Bold.ttf");
    if (err != nullptr) {
        Log::Critical("Pomdog", "failed to load font file");
    }

    std::array<std::pair<FontWeight, FontSize>, 9> fontPairs = {{
        {FontWeight::Light, FontSize::Small},
        {FontWeight::Light, FontSize::Medium},
        {FontWeight::Light, FontSize::Large},
        {FontWeight::Normal, FontSize::Small},
        {FontWeight::Normal, FontSize::Medium},
        {FontWeight::Normal, FontSize::Large},
        {FontWeight::Bold, FontSize::Small},
        {FontWeight::Bold, FontSize::Medium},
        {FontWeight::Bold, FontSize::Large},
    }};

    for (auto& pair : fontPairs) {
        const auto fontWeight = pair.first;
        const auto fontSize = pair.second;

        auto font = fontRegular;
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
            fontPointSize = 16;
            break;
        case FontSize::Large:
            fontPointSize = 26;
            break;
        }

        auto fontID = MakeFontID(fontWeight, fontSize);
        auto spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, fontPointSize, fontPointSize);
        spriteFont->PrepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,-+*/?&!");
        spriteFont->SetDefaultCharacter(U'?');
        spriteFonts.emplace(std::move(fontID), std::move(spriteFont));
    }
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

//std::shared_ptr<GraphicsCommandList> DrawingContext::Render()
//{
//    return renderer.Render();
//}

void DrawingContext::Reset(int viewportWidthIn, int viewportHeightIn)
{
    //    renderer.Reset();

    viewportWidth = viewportWidthIn;
    viewportHeight = viewportHeightIn;

    //    auto viewMatrix = Matrix4x4::CreateTranslation(Vector3{
    //        -viewportWidth * 0.5f,
    //        -viewportHeight * 0.5f,
    //        0.0f
    //    });
    //
    //    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
    //        viewportWidth,
    //        viewportHeight,
    //        0.1f,
    //        100.0f);

    //    renderer.SetViewMatrix(viewMatrix);
    //    renderer.SetProjectionMatrix(projectionMatrix);
}

std::shared_ptr<SpriteFont>
DrawingContext::GetFont(FontWeight fontWeight, FontSize fontSize) const
{
    auto fontID = MakeFontID(fontWeight, fontSize);
    auto iter = spriteFonts.find(fontID);

    POMDOG_ASSERT(iter != std::end(spriteFonts));
    if (iter != std::end(spriteFonts)) {
        return iter->second;
    }
    return nullptr;
}

void DrawingContext::PushScissorRect(const Rectangle& scissorRect)
{
    auto parentRect = Rectangle{0, 0, viewportWidth, viewportHeight};
    if (!scissorRects.empty()) {
        parentRect = scissorRects.back();
    }

    auto rect = scissorRect;

    // NOTE: Flip Y-coordinate
    rect.Y = viewportHeight - scissorRect.Y - scissorRect.Height;

//    rect.X = std::max(scissorRect.X, parentRect.X);
//    rect.Y = std::max(scissorRect.Y, parentRect.Y);
//    rect.Width = (scissorRect.Width - std::max(parentRect.X - scissorRect.X, 0));
//    rect.Height = (scissorRect.Height - std::max(parentRect.Y - scissorRect.Y, 0));
//
//    if (rect.X > parentRect.GetRight()) {
//        rect.Width = 0;
//    }
//    if (rect.Y > parentRect.GetTop()) {
//        rect.Height = 0;
//    }

    if (rect.X < 0) {
        rect.Width = std::max(rect.Width + rect.X, 0);
        rect.X = 0;
    }
    else if (rect.X > viewportWidth) {
        rect.X = viewportWidth;
        rect.Width = 0;
    }
    else if (rect.X + rect.Width > viewportWidth) {
        auto diff = (rect.X + rect.Width) - viewportWidth;
        rect.Width = rect.Width - diff;
    }

    if (rect.Y < 0) {
        rect.Height = std::max(rect.Height + rect.Y, 0);
        rect.Y = 0;
    }
    else if (rect.Y > viewportHeight) {
        rect.Y = viewportHeight;
        rect.Height = 0;
    }
    else if (rect.Y + rect.Height > viewportHeight) {
        auto diff = (rect.Y + rect.Height) - viewportHeight;
        rect.Height = rect.Height - diff;
    }

    primitiveBatch->Flush();
    spriteBatch->Flush();

    commandList->SetScissorRect(rect);
    scissorRects.push_back(rect);
}

void DrawingContext::PopScissorRect()
{
    POMDOG_ASSERT(!scissorRects.empty());
    scissorRects.pop_back();

    auto scissorRect = Rectangle{0, 0, viewportWidth, viewportHeight};
    if (!scissorRects.empty()) {
        scissorRect = scissorRects.back();
    }
    commandList->SetScissorRect(scissorRect);
}

void DrawingContext::BeginDraw(
    const std::shared_ptr<GraphicsCommandList>& commandListIn,
    const Matrix4x4& transformMatrix)
{
    commandList = commandListIn;

    primitiveBatch->Begin(commandList, transformMatrix);
    spriteBatch->Begin(commandList, transformMatrix);
}

void DrawingContext::EndDraw()
{
    primitiveBatch->End();
    spriteBatch->End();
}

PrimitiveBatch* DrawingContext::GetPrimitiveBatch()
{
    return primitiveBatch.get();
}

SpriteBatch* DrawingContext::GetSpriteBatch()
{
    return spriteBatch.get();
}

} // namespace Pomdog::UI
