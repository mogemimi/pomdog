// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/GUI/TextBlock.hpp"
#include "Pomdog/Experimental/GUI/DrawingContext.hpp"
#include "Pomdog/Experimental/GUI/PointerPoint.hpp"
#include "Pomdog/Experimental/GUI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/GUI/UIHelper.hpp"
#include "Pomdog/Math/Math.hpp"

namespace Pomdog::GUI {

TextBlock::TextBlock(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , textColor(Color::Black)
    , baselineHeight(0.0f)
    , fontWeight(FontWeight::Normal)
    , fontSize(FontSize::Medium)
    , textAlignment(TextAlignment::Left)
    , horizontalAlignment(HorizontalAlignment::Stretch)
{
    SetSize(50, 12);
    SetInteractable(false);
}

void TextBlock::SetColor(const Color& color)
{
    textColor = color;
}

void TextBlock::SetFontWeight(FontWeight fontWeightIn)
{
    fontWeight = fontWeightIn;
}

void TextBlock::SetFontSize(FontSize fontSizeIn)
{
    fontSize = fontSizeIn;
}

std::string TextBlock::GetText() const
{
    return this->text;
}

void TextBlock::SetText(const std::string& textIn)
{
    this->text = textIn;
}

void TextBlock::SetBaselineHeight(float pixelSize)
{
    this->baselineHeight = pixelSize;
}

void TextBlock::SetTextAlignment(TextAlignment textAlign)
{
    this->textAlignment = textAlign;
}

void TextBlock::SetHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    if (horizontalAlignment == horizontalAlignmentIn) {
        return;
    }
    horizontalAlignment = horizontalAlignmentIn;
    if (auto parent = GetParent()) {
        parent->MarkContentLayoutDirty();
    }
}

HorizontalAlignment TextBlock::GetHorizontalAlignment() const noexcept
{
    return horizontalAlignment;
}

VerticalAlignment TextBlock::GetVerticalAlignment() const noexcept
{
    return VerticalAlignment::Top;
}

void TextBlock::Draw(DrawingContext& drawingContext)
{
    auto spriteBatch = drawingContext.GetSpriteBatch();
    auto spriteFont = drawingContext.GetFont(fontWeight, fontSize);
    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());

    auto position = MathHelper::ToVector2(globalPos) + Vector2{0.0f, baselineHeight};
    auto originPivot = Vector2::Zero;

    switch (textAlignment) {
    case TextAlignment::Left:
        break;
    case TextAlignment::Center:
        position.X += (static_cast<float>(GetWidth()) * 0.5f);
        originPivot.X = 0.5f;
        break;
    case TextAlignment::Right:
        position.X += static_cast<float>(GetWidth());
        originPivot.X = 1.0f;
        break;
    }

    spriteFont->Draw(
        *spriteBatch,
        text,
        position,
        textColor,
        0.0f,
        originPivot,
        1.0f);

    spriteBatch->Flush();
}

} // namespace Pomdog::GUI
