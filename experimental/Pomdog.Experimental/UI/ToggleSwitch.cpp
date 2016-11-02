// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "ToggleSwitch.hpp"
#include "DrawingContext.hpp"
#include "PointerPoint.hpp"
#include "UIHelper.hpp"
#include "UIEventDispatcher.hpp"

namespace Pomdog {
namespace UI {

ToggleSwitch::ToggleSwitch(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : UIElement(dispatcher)
    , button(50, 18)
    , onContent(u8"On")
    , offContent(u8"Off")
    , isEnabled(true)
    , isOn(true)
{
    SetSize(50, 18);
    SetCursor(MouseCursor::PointingHand);
}

// MARK: - Properties

bool ToggleSwitch::IsOn() const
{
    return isOn;
}

void ToggleSwitch::SetOn(bool isOnIn)
{
    this->isOn = isOnIn;
}

bool ToggleSwitch::IsEnabled() const
{
    return isEnabled;
}

void ToggleSwitch::SetEnabled(bool isEnabledIn)
{
    this->isEnabled = isEnabledIn;
    if (isEnabled) {
        SetCursor(MouseCursor::PointingHand);
    }
    else {
        ResetCursor();
    }
}

std::string ToggleSwitch::GetOnContent() const
{
    return this->onContent;
}

void ToggleSwitch::SetOnContent(const std::string& onContentIn)
{
    this->onContent = onContentIn;
}

std::string ToggleSwitch::GetOffContent() const
{
    return this->offContent;
}

void ToggleSwitch::SetOffContent(const std::string& offContentIn)
{
    this->offContent = offContentIn;
}

HorizontalAlignment ToggleSwitch::GetHorizontalAlignment() const noexcept
{
    return UI::HorizontalAlignment::Stretch;
}

VerticalAlignment ToggleSwitch::GetVerticalAlignment() const noexcept
{
    return UI::VerticalAlignment::Top;
}

// MARK: - Events

void ToggleSwitch::OnEnter()
{
    auto dispatcher = GetDispatcher();
    connection = dispatcher->Connect(shared_from_this());
}

void ToggleSwitch::OnPointerPressed(const PointerPoint& pointerPoint)
{
}

void ToggleSwitch::OnPointerReleased(const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }
    isOn = !isOn;
    Toggled(isOn);
}

void ToggleSwitch::OnRenderSizeChanged(int widthIn, int heightIn)
{
    SetSize(widthIn, heightIn);
}

void ToggleSwitch::Draw(DrawingContext & drawingContext)
{
    auto transform = GetTransform() * drawingContext.Top();

    drawingContext.Push(Matrix3x2::CreateTranslation(Vector2(GetWidth() - button.Width, 0.0f)) * transform);
    button.Draw(drawingContext, isOn, isEnabled);
    drawingContext.Pop();

    if (isOn) {
        drawingContext.DrawString(transform * Matrix3x2::CreateTranslation(Vector2(0, 2)),
            {181,181,181,255}, FontWeight::Bold, FontSize::Medium, onContent);
    }
    else {
        drawingContext.DrawString(transform * Matrix3x2::CreateTranslation(Vector2(0, 2)),
            {181,181,181,255}, FontWeight::Bold, FontSize::Medium, offContent);
    }
}

ToggleSwitch::ToggleSwitchButton::ToggleSwitchButton(int widthIn, int heightIn)
    : Width(widthIn)
    , Height(heightIn)
{
}

void ToggleSwitch::ToggleSwitchButton::Draw(DrawingContext & drawingContext, bool isOnIn, bool isEnabledIn)
{
    auto transform = drawingContext.Top();

    Color backgroundColor = {121, 121, 121, 255};
    Color innerColor = {109, 109, 109, 255};
    Color thumbColor = Color::White;

    const Color toggleOnInnerColor = {4, 125, 137, 255};
    const Color disabledInnerColor = {81, 81, 81, 255};
    const Color disabledThumbColor = {137, 137, 137, 255};

    constexpr int lineThickness = 2;

    if (!isEnabledIn) {
        innerColor = disabledInnerColor;
        thumbColor = disabledThumbColor;
    }

    if (isOnIn && isEnabledIn) {
        innerColor = toggleOnInnerColor;
    }

    drawingContext.DrawRectangle(transform, backgroundColor, Rectangle(0, 0, Width, lineThickness));
    drawingContext.DrawRectangle(transform, backgroundColor, Rectangle(0, Height - lineThickness, Width, lineThickness));

    constexpr int innerLineThickness = 1;
    drawingContext.DrawRectangle(transform, innerColor, Rectangle(lineThickness + innerLineThickness, lineThickness + innerLineThickness,
        Width - (lineThickness * 2 + innerLineThickness * 2),
        Height - (lineThickness * 2 + innerLineThickness * 2)));

    constexpr int thumbWidth = 12;
    if (isOnIn) {
        drawingContext.DrawRectangle(transform, backgroundColor, Rectangle(0, 0, lineThickness, Height));
        drawingContext.DrawRectangle(transform, thumbColor, Rectangle(Width - thumbWidth, 0, thumbWidth, Height));
    }
    else {
        drawingContext.DrawRectangle(transform, backgroundColor, Rectangle(Width - lineThickness, 0, lineThickness, Height));
        drawingContext.DrawRectangle(transform, thumbColor, Rectangle(0, 0, thumbWidth, Height));
    }
}

} // namespace UI
} // namespace Pomdog
