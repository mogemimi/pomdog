// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include "TextBlock.hpp"
#include "DrawingContext.hpp"
#include "PointerPoint.hpp"
#include "UIHelper.hpp"
#include "UIEventDispatcher.hpp"

namespace Pomdog {
namespace UI {
//-----------------------------------------------------------------------
TextBlock::TextBlock(std::shared_ptr<UIEventDispatcher> const& dispatcher)
    : UIElement(dispatcher)
{
    SetSize(50, 15);
}
//-----------------------------------------------------------------------
// MARK: - Properties
//-----------------------------------------------------------------------
std::string TextBlock::Text() const
{
    return this->text;
}
//-----------------------------------------------------------------------
void TextBlock::Text(std::string const& textIn)
{
    this->text = textIn;
}
//-----------------------------------------------------------------------
// MARK: - Events
//-----------------------------------------------------------------------
void TextBlock::OnRenderSizeChanged(int widthIn, int heightIn)
{
    SetSize(widthIn, heightIn);
}
//-----------------------------------------------------------------------
void TextBlock::Draw(DrawingContext & drawingContext)
{
    auto transform = Transform() * drawingContext.Top();

    drawingContext.DrawString(transform, Color::White, FontWeight::Normal, FontSize::Medium, text);
}
//-----------------------------------------------------------------------
} // namespace UI
} // namespace Pomdog
