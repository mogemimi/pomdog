// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/UI/TextBlock.hpp"
#include "Pomdog/Experimental/UI/DrawingContext.hpp"
#include "Pomdog/Experimental/UI/PointerPoint.hpp"
#include "Pomdog/Experimental/UI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/UI/UIHelper.hpp"

namespace Pomdog::UI {

TextBlock::TextBlock(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : UIElement(dispatcher)
    , textColor(Color::Black)
    , fontWeight(FontWeight::Normal)
    , fontSize(FontSize::Medium)
    , horizontalAlignment(UI::HorizontalAlignment::Stretch)
{
    SetSize(50, 12);
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
    return UI::VerticalAlignment::Top;
}

void TextBlock::Draw(DrawingContext& drawingContext)
{
    auto spriteBatch = drawingContext.GetSpriteBatch();
    auto spriteFont = drawingContext.GetFont(fontWeight, fontSize);
    auto globalTransform = GetTransform() * drawingContext.Top();

    auto position = Vector2{globalTransform(2, 0), globalTransform(2, 1)} + Vector2{0.0f, 2.0f};
    spriteFont->Draw(*spriteBatch, text, position, textColor);
}

} // namespace Pomdog::UI
