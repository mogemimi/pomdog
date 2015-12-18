// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include "ToggleSwitch.hpp"
#include "DrawingContext.hpp"
#include "PointerPoint.hpp"
#include "UIHelper.hpp"
#include "UIEventDispatcher.hpp"

namespace Pomdog {
namespace UI {
//-----------------------------------------------------------------------
ToggleSwitch::ToggleSwitch()
    : Control(Matrix3x2::Identity, 50, 18)
    , button(50, 18)
    , onContent(u8"On")
    , offContent(u8"Off")
    , isEnabled(true)
    , isOn(true)
{
    SetCursor(MouseCursor::PointingHand);
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Properties
#endif
//-----------------------------------------------------------------------
bool ToggleSwitch::IsOn() const
{ return isOn; }
//-----------------------------------------------------------------------
void ToggleSwitch::IsOn(bool isOnIn)
{ this->isOn = isOnIn; }
//-----------------------------------------------------------------------
bool ToggleSwitch::IsEnabled() const
{
    return isEnabled;
}
//-----------------------------------------------------------------------
void ToggleSwitch::IsEnabled(bool isEnabledIn)
{
    this->isEnabled = isEnabledIn;
    if (isEnabled) {
        SetCursor(MouseCursor::PointingHand);
    }
    else {
        ResetCursor();
    }
}
//-----------------------------------------------------------------------
std::string ToggleSwitch::OnContent() const
{ return this->onContent; }
//-----------------------------------------------------------------------
void ToggleSwitch::OnContent(std::string const& onContentIn)
{ this->onContent = onContentIn; }
//-----------------------------------------------------------------------
std::string ToggleSwitch::OffContent() const
{ return this->offContent; }
//-----------------------------------------------------------------------
void ToggleSwitch::OffContent(std::string const& offContentIn)
{ this->offContent = offContentIn; }
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Events
#endif
//-----------------------------------------------------------------------
void ToggleSwitch::OnParentChanged()
{
    auto parent = Parent().lock();

    POMDOG_ASSERT(parent);
    POMDOG_ASSERT(!parent->Dispatcher().expired());

    if (auto dispatcher = parent->Dispatcher().lock())
    {
        connection = dispatcher->Connect(shared_from_this());
    }
}
//-----------------------------------------------------------------------
void ToggleSwitch::OnPointerPressed(PointerPoint const& pointerPoint)
{
}
//-----------------------------------------------------------------------
void ToggleSwitch::OnPointerReleased(PointerPoint const& pointerPoint)
{
    if (!isEnabled) {
        return;
    }
    isOn = !isOn;
    Toggled(isOn);
}
//-----------------------------------------------------------------------
void ToggleSwitch::OnRenderSizeChanged(std::uint32_t widthIn, std::uint32_t heightIn)
{
    Width(widthIn);
    Height(heightIn);
}
//-----------------------------------------------------------------------
void ToggleSwitch::Draw(DrawingContext & drawingContext)
{
    auto transform = Transform() * drawingContext.Top();

    drawingContext.Push(Matrix3x2::CreateTranslation(Vector2(Width() - button.Width, 0.0f)) * transform);
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
//-----------------------------------------------------------------------
ToggleSwitch::ToggleSwitchButton::ToggleSwitchButton(std::uint16_t widthIn, std::uint16_t heightIn)
    : Width(widthIn)
    , Height(heightIn)
{
}
//-----------------------------------------------------------------------
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

    if (!isEnabledIn)
    {
        innerColor = disabledInnerColor;
        thumbColor = disabledThumbColor;
    }

    if (isOnIn && isEnabledIn)
    {
        innerColor = toggleOnInnerColor;
    }

    drawingContext.DrawRectangle(transform, backgroundColor, Rectangle(0, 0, Width, lineThickness));
    drawingContext.DrawRectangle(transform, backgroundColor, Rectangle(0, Height - lineThickness, Width, lineThickness));

    constexpr int innerLineThickness = 1;
    drawingContext.DrawRectangle(transform, innerColor, Rectangle(lineThickness + innerLineThickness, lineThickness + innerLineThickness,
        Width - (lineThickness * 2 + innerLineThickness * 2),
        Height - (lineThickness * 2 + innerLineThickness * 2)));

    constexpr std::uint16_t thumbWidth = 12;
    if (isOnIn) {
        drawingContext.DrawRectangle(transform, backgroundColor, Rectangle(0, 0, lineThickness, Height));
        drawingContext.DrawRectangle(transform, thumbColor, Rectangle(Width - thumbWidth, 0, thumbWidth, Height));
    }
    else {
        drawingContext.DrawRectangle(transform, backgroundColor, Rectangle(Width - lineThickness, 0, lineThickness, Height));
        drawingContext.DrawRectangle(transform, thumbColor, Rectangle(0, 0, thumbWidth, Height));
    }
}
//-----------------------------------------------------------------------
}// namespace UI
}// namespace Pomdog
