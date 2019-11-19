// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/UI/PushButton.hpp"
#include "Pomdog/Experimental/Tween/EasingHelper.hpp"
#include "Pomdog/Experimental/UI/DrawingContext.hpp"
#include "Pomdog/Experimental/UI/PointerPoint.hpp"
#include "Pomdog/Experimental/UI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/UI/UIHelper.hpp"

namespace Pomdog::UI {

PushButton::PushButton(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : UIElement(dispatcher)
    , isEnabled(true)
    , isHovered(false)
    , isPressed(false)
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

void PushButton::SetText(const std::string& textIn)
{
    this->text = textIn;
}

HorizontalAlignment PushButton::GetHorizontalAlignment() const noexcept
{
    return UI::HorizontalAlignment::Right;
}

VerticalAlignment PushButton::GetVerticalAlignment() const noexcept
{
    return UI::VerticalAlignment::Top;
}

void PushButton::OnEnter()
{
    auto dispatcher = GetDispatcher();
    connection = dispatcher->Connect(shared_from_this());
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

    auto globalTransform = GetTransform() * drawingContext.Top();
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
        globalTransform,
        Vector2{0.0f, 0.0f},
        static_cast<float>(GetWidth()),
        static_cast<float>(GetHeight()),
        rectColor);

    primitiveBatch->Flush();

    if (!text.empty()) {
        auto spriteBatch = drawingContext.GetSpriteBatch();
        auto spriteFont = drawingContext.GetFont(FontWeight::Bold, FontSize::Medium);

        auto buttonPos = Vector2{globalTransform(2, 0), globalTransform(2, 1)};
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
    }
}

} // namespace Pomdog::UI
