// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/text_block.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/math/math_functions.h"

namespace pomdog::gui {

TextBlock::TextBlock(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , textColor_(Color::createBlack())
    , baselineHeight_(0.0f)
    , fontWeight_(FontWeight::Normal)
    , fontSize_(FontSize::Medium)
    , textAlignment_(TextAlignment::Left)
    , horizontalAlignment_(HorizontalAlignment::Stretch)
{
    setSize(50, 12);
    setInteractable(false);
}

void TextBlock::setColor(const Color& color)
{
    textColor_ = color;
}

void TextBlock::setFontWeight(FontWeight fontWeightIn)
{
    fontWeight_ = fontWeightIn;
}

void TextBlock::setFontSize(FontSize fontSizeIn)
{
    fontSize_ = fontSizeIn;
}

std::string TextBlock::getText() const
{
    return text_;
}

void TextBlock::setText(const std::string& textIn)
{
    text_ = textIn;
}

void TextBlock::setBaselineHeight(float pixelSize)
{
    baselineHeight_ = pixelSize;
}

void TextBlock::setTextAlignment(TextAlignment textAlign)
{
    textAlignment_ = textAlign;
}

void TextBlock::setHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    if (horizontalAlignment_ == horizontalAlignmentIn) {
        return;
    }
    horizontalAlignment_ = horizontalAlignmentIn;
    if (auto parent = getParent()) {
        parent->markContentLayoutDirty();
    }
}

HorizontalAlignment TextBlock::getHorizontalAlignment() const noexcept
{
    return horizontalAlignment_;
}

VerticalAlignment TextBlock::getVerticalAlignment() const noexcept
{
    return VerticalAlignment::Top;
}

void TextBlock::draw(DrawingContext& drawingContext)
{
    auto spriteBatch = drawingContext.getSpriteBatch();
    auto spriteFont = drawingContext.getFont(fontWeight_, fontSize_);
    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());

    auto position = math::toVector2(globalPos) + Vector2{0.0f, baselineHeight_};
    auto originPivot = Vector2::createZero();

    switch (textAlignment_) {
    case TextAlignment::Left:
        break;
    case TextAlignment::Center:
        position.x += (static_cast<float>(getWidth()) * 0.5f);
        originPivot.x = 0.5f;
        break;
    case TextAlignment::Right:
        position.x += static_cast<float>(getWidth());
        originPivot.x = 1.0f;
        break;
    }

    spriteFont->draw(
        *spriteBatch,
        text_,
        position,
        textColor_,
        0.0f,
        originPivot,
        1.0f);

    spriteBatch->flush();
}

} // namespace pomdog::gui
