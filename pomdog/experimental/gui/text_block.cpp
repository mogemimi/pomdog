// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/text_block.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/math/math.h"

namespace pomdog::gui {

TextBlock::TextBlock(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , textColor(Color::createBlack())
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

    auto position = math::toVector2(globalPos) + Vector2{0.0f, baselineHeight};
    auto originPivot = Vector2::createZero();

    switch (textAlignment) {
    case TextAlignment::Left:
        break;
    case TextAlignment::Center:
        position.x += (static_cast<float>(GetWidth()) * 0.5f);
        originPivot.x = 0.5f;
        break;
    case TextAlignment::Right:
        position.x += static_cast<float>(GetWidth());
        originPivot.x = 1.0f;
        break;
    }

    spriteFont->draw(
        *spriteBatch,
        text,
        position,
        textColor,
        0.0f,
        originPivot,
        1.0f);

    spriteBatch->flush();
}

} // namespace pomdog::gui
