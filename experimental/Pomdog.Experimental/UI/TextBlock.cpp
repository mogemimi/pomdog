// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/UI/TextBlock.hpp"
#include "Pomdog.Experimental/UI/DrawingContext.hpp"
#include "Pomdog.Experimental/UI/PointerPoint.hpp"
#include "Pomdog.Experimental/UI/UIHelper.hpp"
#include "Pomdog.Experimental/UI/UIEventDispatcher.hpp"

namespace Pomdog {
namespace UI {

TextBlock::TextBlock(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : UIElement(dispatcher)
    , fontWeight(FontWeight::Normal)
    , fontSize(FontSize::Medium)
    , horizontalAlignment(UI::HorizontalAlignment::Stretch)
{
    SetSize(50, 12);
    spriteCommand.SetScale(Vector2{1.0f, 1.0f});
    spriteCommand.SetRotation(0.0f);
    spriteCommand.SetColor(Color{232, 231, 230, 255});
}

// MARK: - Properties

void TextBlock::SetColor(const Color& color)
{
    spriteCommand.SetColor(color);
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
    spriteCommand.SetText(text);
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

// MARK: - Events

void TextBlock::Draw(DrawingContext & drawingContext)
{
    auto transform = GetTransform() * drawingContext.Top();
    spriteCommand.SetFont(drawingContext.GetFont(fontWeight, fontSize));
    spriteCommand.SetPosition(Vector2{transform(2, 0), transform(2, 1)} + Vector2{0.0f, 2.0f});
    spriteCommand.SetDrawOrder(5.0f);
    drawingContext.PushCommand(spriteCommand);
}

} // namespace UI
} // namespace Pomdog
