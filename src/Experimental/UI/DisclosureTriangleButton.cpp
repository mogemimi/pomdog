// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/UI/DisclosureTriangleButton.hpp"
#include "Pomdog/Experimental/UI/DrawingContext.hpp"
#include "Pomdog/Experimental/UI/PointerPoint.hpp"
#include "Pomdog/Experimental/UI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/UI/UIHelper.hpp"

namespace Pomdog::UI {

DisclosureTriangleButton::DisclosureTriangleButton(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : UIElement(dispatcher)
    , isOn(true)
    , isEnabled(true)
{
    SetSize(8, 8);
    SetCursor(MouseCursor::PointingHand);
}

// MARK: - Properties

bool DisclosureTriangleButton::IsOn() const
{
    return isOn;
}

void DisclosureTriangleButton::SetOn(bool isOnIn)
{
    this->isOn = isOnIn;
}

bool DisclosureTriangleButton::IsEnabled() const
{
    return isEnabled;
}

void DisclosureTriangleButton::SetEnabled(bool isEnabledIn)
{
    this->isEnabled = isEnabledIn;
    if (isEnabled) {
        SetCursor(MouseCursor::PointingHand);
    }
    else {
        ResetCursor();
    }
}

HorizontalAlignment DisclosureTriangleButton::GetHorizontalAlignment() const noexcept
{
    return UI::HorizontalAlignment::Left;
}

VerticalAlignment DisclosureTriangleButton::GetVerticalAlignment() const noexcept
{
    return UI::VerticalAlignment::Top;
}

// MARK: - Events

void DisclosureTriangleButton::OnEnter()
{
    auto dispatcher = GetDispatcher();
    connection = dispatcher->Connect(shared_from_this());
}

void DisclosureTriangleButton::OnPointerPressed([[maybe_unused]] const PointerPoint& pointerPoint)
{
}

void DisclosureTriangleButton::OnPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }
    isOn = !isOn;
    Toggled(isOn);
}

void DisclosureTriangleButton::Draw(DrawingContext& drawingContext)
{
    auto globalTransform = GetTransform() * drawingContext.Top();
    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    Color thumbColor;
    if (isOn) {
        thumbColor = Color{27, 161, 226, 255};
    }
    else {
        thumbColor = Color{162, 160, 161, 255};
    }

    const auto transformOffset = Vector2{globalTransform(2, 0), globalTransform(2, 1)};

    constexpr float triangleWidth = 7.0f;
    constexpr float triangleHeight = triangleWidth * 1.73205f * 0.5f;
    if (isOn) {
        primitiveBatch->DrawTriangle(
            transformOffset + Vector2{triangleWidth * 0.5f, 0.0f},
            transformOffset + Vector2{0.0f, triangleHeight},
            transformOffset + Vector2{triangleWidth, triangleHeight},
            thumbColor);
    }
    else {
        primitiveBatch->DrawTriangle(
            transformOffset + Vector2{0.0f, 0.0f},
            transformOffset + Vector2{0.0f, triangleWidth},
            transformOffset + Vector2{triangleHeight, triangleWidth * 0.5f},
            thumbColor);
    }
    primitiveBatch->Flush();
}

} // namespace Pomdog::UI
