// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/GUI/PushButton.hpp"
#include "Pomdog/Experimental/GUI/DrawingContext.hpp"
#include "Pomdog/Experimental/GUI/PointerPoint.hpp"
#include "Pomdog/Experimental/GUI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/GUI/UIHelper.hpp"
#include "Pomdog/Experimental/Tween/EasingHelper.hpp"

namespace Pomdog::GUI {

PushButton::PushButton(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
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

void PushButton::SetText(const std::string& textIn)
{
    this->text = textIn;
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
    const Color textNormalColor = {251, 250, 248, 255};
    const Color textDisabledColor = {191, 190, 189, 255};
    const Color rectNormalColor = {0, 132, 190, 255};
    const Color rectHoveredColor = {0, 123, 182, 255};
    const Color rectClickColor = {0, 107, 162, 255};
    const Color rectDisabledColor = {110, 108, 107, 255};

    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());
    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    auto textColor = textNormalColor;
    auto rectColor = rectNormalColor;

    if (!isEnabled) {
        textColor = textDisabledColor;
        rectColor = rectDisabledColor;
    }
    else if (isPressed) {
        textColor = textNormalColor;
        rectColor = rectClickColor;
    }
    else if (isHovered) {
        textColor = textNormalColor;
        rectColor = rectHoveredColor;
    }

    primitiveBatch->DrawRectangle(
        Matrix3x2::Identity,
        MathHelper::ToVector2(globalPos),
        static_cast<float>(GetWidth()),
        static_cast<float>(GetHeight()),
        rectColor);

    primitiveBatch->Flush();

    if (!text.empty()) {
        auto spriteBatch = drawingContext.GetSpriteBatch();
        auto spriteFont = drawingContext.GetFont(FontWeight::Bold, FontSize::Medium);

        auto buttonPos = MathHelper::ToVector2(globalPos);
        auto buttonSize = Vector2{static_cast<float>(GetWidth()), static_cast<float>(GetHeight())};
        auto textPosition = buttonPos + buttonSize * 0.5f + Vector2{0.0f, 3.0f};
        spriteFont->Draw(
            *spriteBatch,
            text,
            textPosition,
            textColor,
            0.0f,
            Vector2{0.5f, 0.5f},
            1.0f);

        spriteBatch->Flush();
    }
}

} // namespace Pomdog::GUI
