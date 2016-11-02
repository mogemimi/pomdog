// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "TextBlock.hpp"
#include "DrawingContext.hpp"
#include "PointerPoint.hpp"
#include "UIHelper.hpp"
#include "UIEventDispatcher.hpp"

namespace Pomdog {
namespace UI {

TextBlock::TextBlock(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : UIElement(dispatcher)
{
    SetSize(50, 15);
}

// MARK: - Properties

std::string TextBlock::Text() const
{
    return this->text;
}

void TextBlock::Text(const std::string& textIn)
{
    this->text = textIn;
}

// MARK: - Events

void TextBlock::OnRenderSizeChanged(int widthIn, int heightIn)
{
    SetSize(widthIn, heightIn);
}

void TextBlock::Draw(DrawingContext & drawingContext)
{
    auto transform = Transform() * drawingContext.Top();

    drawingContext.DrawString(transform, Color::White, FontWeight::Normal, FontSize::Medium, text);
}

} // namespace UI
} // namespace Pomdog
