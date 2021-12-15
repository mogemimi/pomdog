// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/push_button.hpp"
#include "pomdog/experimental/gui/drawing_context.hpp"
#include "pomdog/experimental/gui/pointer_point.hpp"
#include "pomdog/experimental/gui/ui_event_dispatcher.hpp"
#include "pomdog/experimental/gui/ui_helper.hpp"
#include "pomdog/experimental/tween/easing_helper.hpp"

namespace pomdog::gui {

PushButton::PushButton(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , fontWeight(FontWeight::Bold)
    , textAlignment(TextAlignment::Center)
    , horizontalAlignment(HorizontalAlignment::Stretch)
    , verticalAlignment(VerticalAlignment::Top)
    , isEnabled(true)
    , isHovered(false)
    , isPressed(false)
    , isFocused(false)
{
    SetSize(74, 24);
    SetCursor(MouseCursor::PointingHand);
}

bool PushButton::IsEnabled() const
{
    return isEnabled;
}

void PushButton::SetEnabled(bool isEnabledIn)
{
    this->isEnabled = isEnabledIn;
    if (isEnabled) {
        SetCursor(MouseCursor::PointingHand);
    }
    else {
        ResetCursor();
    }
}

bool PushButton::IsHovered() const
{
    return isHovered;
}

bool PushButton::IsFocused() const
{
    return isFocused;
}

void PushButton::SetFontWeight(FontWeight fontWeightIn)
{
    this->fontWeight = fontWeightIn;
}

std::string PushButton::GetText() const
{
    return text;
}

void PushButton::SetText(const std::string& textIn)
{
    this->text = textIn;
}

void PushButton::SetTextAlignment(TextAlignment textAlign)
{
    this->textAlignment = textAlign;
}

void PushButton::SetHorizontalAlignment(HorizontalAlignment horizontalAlignmentIn) noexcept
{
    this->horizontalAlignment = horizontalAlignmentIn;
}

void PushButton::SetVerticalAlignment(VerticalAlignment verticalAlignmentIn) noexcept
{
    this->verticalAlignment = verticalAlignmentIn;
}

HorizontalAlignment PushButton::GetHorizontalAlignment() const noexcept
{
    return horizontalAlignment;
}

VerticalAlignment PushButton::GetVerticalAlignment() const noexcept
{
    return verticalAlignment;
}

void PushButton::OnEnter()
{
}

void PushButton::OnFocusIn()
{
    isFocused = true;
}

void PushButton::OnFocusOut()
{
    isFocused = false;
}

void PushButton::OnPointerEntered([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }
    isHovered = true;
}

void PushButton::OnPointerExited([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isHovered = false;
}

void PushButton::OnPointerPressed([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isPressed = true;
}

void PushButton::OnPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }
    isPressed = false;
    Click();
}

void PushButton::Draw(DrawingContext& drawingContext)
{
    const auto* colorScheme = drawingContext.GetColorScheme();
    POMDOG_ASSERT(colorScheme != nullptr);

    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());
    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    auto textColor = colorScheme->PushButtonTextColorBase;
    auto rectColor = colorScheme->PushButtonRectColorBase;

    if (!isEnabled) {
        textColor = colorScheme->PushButtonTextColorDisabled;
        rectColor = colorScheme->PushButtonRectColorDisabled;
    }
    else if (isPressed) {
        textColor = colorScheme->PushButtonTextColorBase;
        rectColor = colorScheme->PushButtonRectColorClick;
    }
    else if (isHovered) {
        textColor = colorScheme->PushButtonTextColorBase;
        rectColor = colorScheme->PushButtonRectColorHovered;
    }

    primitiveBatch->DrawRectangle(
        Matrix3x2::Identity(),
        math::ToVector2(globalPos),
        static_cast<float>(GetWidth()),
        static_cast<float>(GetHeight()),
        rectColor);

    primitiveBatch->Flush();

    if (!text.empty()) {
        auto spriteBatch = drawingContext.GetSpriteBatch();
        auto spriteFont = drawingContext.GetFont(fontWeight, FontSize::Medium);

        const auto buttonPos = math::ToVector2(globalPos);
        const auto buttonSize = Vector2{static_cast<float>(GetWidth()), static_cast<float>(GetHeight())};

        const auto baselineHeight = 3.0f;
        const auto horizontalPadding = 6.0f;

        auto originPivot = Vector2::Zero();
        Vector2 padding = Vector2{0.0f, baselineHeight};
        switch (textAlignment) {
        case TextAlignment::Left:
            originPivot = Vector2{0.0f, 0.5f};
            padding.X = horizontalPadding;
            break;
        case TextAlignment::Center:
            originPivot = Vector2{0.5f, 0.5f};
            break;
        case TextAlignment::Right:
            originPivot = Vector2{1.0f, 0.5f};
            padding.X = -horizontalPadding;
            break;
        }

        const auto textPosition = buttonPos + buttonSize * originPivot + padding;

        spriteFont->Draw(
            *spriteBatch,
            text,
            textPosition,
            textColor,
            0.0f,
            originPivot,
            1.0f);

        spriteBatch->Flush();
    }
}

} // namespace pomdog::gui
