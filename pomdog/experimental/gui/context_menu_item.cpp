// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/context_menu_item.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/math/math_functions.h"

namespace pomdog::gui {

ContextMenuButton::ContextMenuButton(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , marginLeft_(15)
    , marginRight_(15)
    , clickInterval_(0)
    , fontWeight_(FontWeight::Normal)
    , textAlignment_(TextAlignment::Left)
    , horizontalAlignment_(HorizontalAlignment::Stretch)
    , verticalAlignment_(VerticalAlignment::Top)
    , isEnabled_(true)
    , isHovered_(false)
    , isPressed_(false)
    , isFocused_(false)
{
    setSize(74, 20);
}

bool ContextMenuButton::isEnabled() const
{
    return isEnabled_;
}

void ContextMenuButton::setEnabled(bool isEnabledIn)
{
    isEnabled_ = isEnabledIn;
}

bool ContextMenuButton::isHovered() const
{
    return isHovered_;
}

bool ContextMenuButton::isFocused() const
{
    return isFocused_;
}

void ContextMenuButton::setFontWeight(FontWeight fontWeightIn)
{
    fontWeight_ = fontWeightIn;
}

std::string ContextMenuButton::getText() const
{
    return text_;
}

void ContextMenuButton::setText(const std::string& textIn)
{
    text_ = textIn;
}

void ContextMenuButton::setTextAlignment(TextAlignment textAlign)
{
    textAlignment_ = textAlign;
}

void ContextMenuButton::setMargin(const Thickness& margin)
{
    static_assert(std::is_same_v<decltype(marginLeft_), i16>);
    static_assert(std::is_same_v<decltype(marginRight_), i16>);

    marginLeft_ = static_cast<i16>(margin.left);
    marginRight_ = static_cast<i16>(margin.right);
}

void ContextMenuButton::setHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    horizontalAlignment_ = horizontalAlignmentIn;
}

void ContextMenuButton::setVerticalAlignment(VerticalAlignment verticalAlignmentIn) noexcept
{
    verticalAlignment_ = verticalAlignmentIn;
}

HorizontalAlignment ContextMenuButton::getHorizontalAlignment() const noexcept
{
    return horizontalAlignment_;
}

VerticalAlignment ContextMenuButton::getVerticalAlignment() const noexcept
{
    return verticalAlignment_;
}

void ContextMenuButton::onEnter()
{
}

void ContextMenuButton::onFocusIn()
{
    isFocused_ = true;
}

void ContextMenuButton::onFocusOut()
{
    isFocused_ = false;
    FocusOut();
}

void ContextMenuButton::onPointerEntered([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled_) {
        return;
    }
    isHovered_ = true;
}

void ContextMenuButton::onPointerExited([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isHovered_ = false;
}

void ContextMenuButton::onPointerPressed([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isPressed_ = true;
}

void ContextMenuButton::onPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled_) {
        return;
    }
    isPressed_ = false;
    if (clickInterval_ <= 0) {
        clickInterval_ = 2;
        Click();
    }
}

void ContextMenuButton::updateAnimation([[maybe_unused]] const Duration& frameDuration)
{
    if (clickInterval_ > 0) {
        clickInterval_--;
    }
    POMDOG_ASSERT(clickInterval_ >= 0);
}

void ContextMenuButton::draw(DrawingContext& drawingContext)
{
    const Color textNormalColor = {251, 250, 248, 255};
    const Color textDisabledColor = {191, 190, 189, 255};
    const Color rectHoveredColor = {0, 123, 182, 255};

    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());
    auto primitiveBatch = drawingContext.getPrimitiveBatch();

    auto textColor = textNormalColor;

    if (!isEnabled_) {
        textColor = textDisabledColor;
    }

    if (isEnabled_ && (isHovered_ || isFocused_) && (clickInterval_ <= 0)) {
        primitiveBatch->drawRectangle(
            Rect2D{globalPos.x, globalPos.y, getWidth(), getHeight()},
            rectHoveredColor);

        drawingContext.flushPrimitiveBatch();
    }

    if (!text_.empty()) {
        auto spriteBatch = drawingContext.getSpriteBatch();
        auto spriteFont = drawingContext.getFont(fontWeight_, FontSize::Medium);

        const auto buttonPos = math::toVector2(globalPos);
        const auto buttonSize = Vector2{static_cast<float>(getWidth()), static_cast<float>(getHeight())};

        const auto baselineHeight = 3.0f;

        Vector2 originPivot = Vector2::createZero();
        Vector2 padding = Vector2{0.0f, baselineHeight};
        switch (textAlignment_) {
        case TextAlignment::Left:
            originPivot = Vector2{0.0f, 0.5f};
            padding.x = static_cast<float>(marginLeft_);
            break;
        case TextAlignment::Center:
            originPivot = Vector2{0.5f, 0.5f};
            break;
        case TextAlignment::Right:
            originPivot = Vector2{1.0f, 0.5f};
            padding.x = -static_cast<float>(marginRight_);
            break;
        }

        const auto textPosition = buttonPos + buttonSize * originPivot + padding;

        spriteFont->draw(
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
