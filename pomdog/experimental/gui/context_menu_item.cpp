// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/context_menu_item.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/math/math.h"

namespace pomdog::gui {

ContextMenuButton::ContextMenuButton(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , marginLeft(15)
    , marginRight(15)
    , clickInterval(0)
    , fontWeight(FontWeight::Normal)
    , textAlignment(TextAlignment::Left)
    , horizontalAlignment(HorizontalAlignment::Stretch)
    , verticalAlignment(VerticalAlignment::Top)
    , isEnabled(true)
    , isHovered(false)
    , isPressed(false)
    , isFocused(false)
{
    SetSize(74, 20);
}

bool ContextMenuButton::IsEnabled() const
{
    return isEnabled;
}

void ContextMenuButton::SetEnabled(bool isEnabledIn)
{
    this->isEnabled = isEnabledIn;
}

bool ContextMenuButton::IsHovered() const
{
    return isHovered;
}

bool ContextMenuButton::IsFocused() const
{
    return isFocused;
}

void ContextMenuButton::SetFontWeight(FontWeight fontWeightIn)
{
    this->fontWeight = fontWeightIn;
}

std::string ContextMenuButton::GetText() const
{
    return text;
}

void ContextMenuButton::SetText(const std::string& textIn)
{
    this->text = textIn;
}

void ContextMenuButton::SetTextAlignment(TextAlignment textAlign)
{
    this->textAlignment = textAlign;
}

void ContextMenuButton::SetMargin(const Thickness& margin)
{
    static_assert(std::is_same_v<decltype(marginLeft), std::int16_t>);
    static_assert(std::is_same_v<decltype(marginRight), std::int16_t>);

    marginLeft = static_cast<std::int16_t>(margin.left);
    marginRight = static_cast<std::int16_t>(margin.right);
}

void ContextMenuButton::SetHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    this->horizontalAlignment = horizontalAlignmentIn;
}

void ContextMenuButton::SetVerticalAlignment(VerticalAlignment verticalAlignmentIn) noexcept
{
    this->verticalAlignment = verticalAlignmentIn;
}

HorizontalAlignment ContextMenuButton::GetHorizontalAlignment() const noexcept
{
    return horizontalAlignment;
}

VerticalAlignment ContextMenuButton::GetVerticalAlignment() const noexcept
{
    return verticalAlignment;
}

void ContextMenuButton::OnEnter()
{
}

void ContextMenuButton::OnFocusIn()
{
    isFocused = true;
}

void ContextMenuButton::OnFocusOut()
{
    isFocused = false;
    FocusOut();
}

void ContextMenuButton::OnPointerEntered([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }
    isHovered = true;
}

void ContextMenuButton::OnPointerExited([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isHovered = false;
}

void ContextMenuButton::OnPointerPressed([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isPressed = true;
}

void ContextMenuButton::OnPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }
    isPressed = false;
    if (clickInterval <= 0) {
        clickInterval = 2;
        Click();
    }
}

void ContextMenuButton::UpdateAnimation([[maybe_unused]] const Duration& frameDuration)
{
    if (clickInterval > 0) {
        clickInterval--;
    }
    POMDOG_ASSERT(clickInterval >= 0);
}

void ContextMenuButton::Draw(DrawingContext& drawingContext)
{
    const Color textNormalColor = {251, 250, 248, 255};
    const Color textDisabledColor = {191, 190, 189, 255};
    const Color rectHoveredColor = {0, 123, 182, 255};

    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());
    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    auto textColor = textNormalColor;

    if (!isEnabled) {
        textColor = textDisabledColor;
    }

    if (isEnabled && (isHovered || isFocused) && (clickInterval <= 0)) {
        primitiveBatch->drawRectangle(
            Rectangle{globalPos.x, globalPos.y, GetWidth(), GetHeight()},
            rectHoveredColor);

        primitiveBatch->flush();
    }

    if (!text.empty()) {
        auto spriteBatch = drawingContext.GetSpriteBatch();
        auto spriteFont = drawingContext.GetFont(fontWeight, FontSize::Medium);

        const auto buttonPos = math::toVector2(globalPos);
        const auto buttonSize = Vector2{static_cast<float>(GetWidth()), static_cast<float>(GetHeight())};

        const auto baselineHeight = 3.0f;

        Vector2 originPivot = Vector2::createZero();
        Vector2 padding = Vector2{0.0f, baselineHeight};
        switch (textAlignment) {
        case TextAlignment::Left:
            originPivot = Vector2{0.0f, 0.5f};
            padding.x = static_cast<float>(marginLeft);
            break;
        case TextAlignment::Center:
            originPivot = Vector2{0.5f, 0.5f};
            break;
        case TextAlignment::Right:
            originPivot = Vector2{1.0f, 0.5f};
            padding.x = -static_cast<float>(marginRight);
            break;
        }

        const auto textPosition = buttonPos + buttonSize * originPivot + padding;

        spriteFont->draw(
            *spriteBatch,
            text,
            textPosition,
            textColor,
            0.0f,
            originPivot,
            1.0f);

        spriteBatch->flush();
    }
}

} // namespace pomdog::gui
