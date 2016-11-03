// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/UI/TextBlock.hpp"
#include "Pomdog.Experimental/UI/DrawingContext.hpp"
#include "Pomdog.Experimental/UI/PointerPoint.hpp"
#include "Pomdog.Experimental/UI/UIHelper.hpp"
#include "Pomdog.Experimental/UI/UIEventDispatcher.hpp"

namespace Pomdog {
namespace UI {

TextBlock::TextBlock(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : UIElement(dispatcher)
{
    SetSize(50, 15);
}

// MARK: - Properties

std::string TextBlock::GetText() const
{
    return this->text;
}

void TextBlock::SetText(const std::string& textIn)
{
    this->text = textIn;
}

HorizontalAlignment TextBlock::GetHorizontalAlignment() const noexcept
{
    return UI::HorizontalAlignment::Stretch;
}

VerticalAlignment TextBlock::GetVerticalAlignment() const noexcept
{
    return UI::VerticalAlignment::Top;
}

// MARK: - Events

void TextBlock::OnRenderSizeChanged(int widthIn, int heightIn)
{
    SetSize(widthIn, heightIn);
}

void TextBlock::Draw(DrawingContext & drawingContext)
{
    auto transform = GetTransform() * drawingContext.Top();

    drawingContext.DrawString(transform, Color::White, FontWeight::Normal, FontSize::Medium, text);
}

} // namespace UI
} // namespace Pomdog
