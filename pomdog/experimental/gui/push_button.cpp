// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/push_button.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"

namespace pomdog::gui {

PushButton::PushButton(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , fontWeight_(FontWeight::Bold)
    , textAlignment_(TextAlignment::Center)
    , horizontalAlignment_(HorizontalAlignment::Stretch)
    , verticalAlignment_(VerticalAlignment::Top)
    , isEnabled_(true)
    , isHovered_(false)
    , isPressed_(false)
    , isFocused_(false)
{
    setSize(74, 24);
    setCursor(MouseCursor::PointingHand);
}

bool PushButton::isEnabled() const
{
    return isEnabled_;
}

void PushButton::setEnabled(bool isEnabledIn)
{
    isEnabled_ = isEnabledIn;
    if (isEnabled_) {
        setCursor(MouseCursor::PointingHand);
    }
    else {
        resetCursor();
    }
}

bool PushButton::isHovered() const
{
    return isHovered_;
}

bool PushButton::isFocused() const
{
    return isFocused_;
}

void PushButton::setFontWeight(FontWeight fontWeightIn)
{
    fontWeight_ = fontWeightIn;
}

std::string PushButton::getText() const
{
    return text_;
}

void PushButton::setText(const std::string& textIn)
{
    text_ = textIn;
}

void PushButton::setTextAlignment(TextAlignment textAlign)
{
    textAlignment_ = textAlign;
}

void PushButton::setHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    horizontalAlignment_ = horizontalAlignmentIn;
}

void PushButton::setVerticalAlignment(VerticalAlignment verticalAlignmentIn) noexcept
{
    verticalAlignment_ = verticalAlignmentIn;
}

HorizontalAlignment PushButton::getHorizontalAlignment() const noexcept
{
    return horizontalAlignment_;
}

VerticalAlignment PushButton::getVerticalAlignment() const noexcept
{
    return verticalAlignment_;
}

void PushButton::onEnter()
{
}

void PushButton::onFocusIn()
{
    isFocused_ = true;
}

void PushButton::onFocusOut()
{
    isFocused_ = false;
}

void PushButton::onPointerEntered([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled_) {
        return;
    }
    isHovered_ = true;
}

void PushButton::onPointerExited([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isHovered_ = false;
}

void PushButton::onPointerPressed([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isPressed_ = true;
}

void PushButton::onPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled_) {
        return;
    }
    isPressed_ = false;
    Click();
}

void PushButton::draw(DrawingContext& drawingContext)
{
    const auto* colorScheme = drawingContext.getColorScheme();
    POMDOG_ASSERT(colorScheme != nullptr);

    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());
    auto primitiveBatch = drawingContext.getPrimitiveBatch();

    auto textColor = colorScheme->PushButtonTextColorBase;
    auto rectColor = colorScheme->PushButtonRectColorBase;

    if (!isEnabled_) {
        textColor = colorScheme->PushButtonTextColorDisabled;
        rectColor = colorScheme->PushButtonRectColorDisabled;
    }
    else if (isPressed_) {
        textColor = colorScheme->PushButtonTextColorBase;
        rectColor = colorScheme->PushButtonRectColorClick;
    }
    else if (isHovered_) {
        textColor = colorScheme->PushButtonTextColorBase;
        rectColor = colorScheme->PushButtonRectColorHovered;
    }

    primitiveBatch->drawRectangle(
        Matrix3x2::createIdentity(),
        math::toVector2(globalPos),
        static_cast<float>(getWidth()),
        static_cast<float>(getHeight()),
        rectColor);

    drawingContext.flushPrimitiveBatch();

    if (!text_.empty()) {
        auto spriteBatch = drawingContext.getSpriteBatch();
        auto spriteFont = drawingContext.getFont(fontWeight_, FontSize::Medium);

        const auto buttonPos = math::toVector2(globalPos);
        const auto buttonSize = Vector2{static_cast<float>(getWidth()), static_cast<float>(getHeight())};

        const auto baselineHeight = 3.0f;
        const auto horizontalPadding = 6.0f;

        auto originPivot = Vector2::createZero();
        Vector2 padding = Vector2{0.0f, baselineHeight};
        switch (textAlignment_) {
        case TextAlignment::Left:
            originPivot = Vector2{0.0f, 0.5f};
            padding.x = horizontalPadding;
            break;
        case TextAlignment::Center:
            originPivot = Vector2{0.5f, 0.5f};
            break;
        case TextAlignment::Right:
            originPivot = Vector2{1.0f, 0.5f};
            padding.x = -horizontalPadding;
            break;
        }

        const auto textPosition = buttonPos + buttonSize * originPivot + padding;

        spriteFont->draw(
            drawingContext.getGraphicsDevice(),
            *spriteBatch,
            text_,
            textPosition,
            textColor,
            0.0f,
            originPivot,
            1.0f);

        drawingContext.flushSpriteBatch();
    }
}

} // namespace pomdog::gui
