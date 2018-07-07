// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/UI/DisclosureTriangleButton.hpp"
#include "Pomdog.Experimental/UI/DrawingContext.hpp"
#include "Pomdog.Experimental/UI/PointerPoint.hpp"
#include "Pomdog.Experimental/UI/UIHelper.hpp"
#include "Pomdog.Experimental/UI/UIEventDispatcher.hpp"

namespace Pomdog {
namespace UI {

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

void DisclosureTriangleButton::OnPointerPressed(const PointerPoint& pointerPoint)
{
}

void DisclosureTriangleButton::OnPointerReleased(const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }
    isOn = !isOn;
    Toggled(isOn);
}

void DisclosureTriangleButton::Draw(DrawingContext & drawingContext)
{
    auto transform = GetTransform() * drawingContext.Top();

    renderCommand.SetInvoker([this, transform](PolygonBatch & polygonBatch) {
        Color thumbColor = isOn ? Color{27, 161, 226, 255} : Color{162, 160, 161, 255};

        const auto transformOffset = Vector2{transform(2, 0), transform(2, 1)};

        const auto triangleWidth = 6.0f;
        if (isOn) {
            polygonBatch.DrawTriangle(
                transformOffset + Vector2{0.0f, 0.0f},
                transformOffset + Vector2{triangleWidth, triangleWidth},
                transformOffset + Vector2{triangleWidth, 0.0f},
                thumbColor);
        }
        else {
            const float squareRoot = 1.41421356f;
            polygonBatch.DrawTriangle(
                transformOffset + Vector2{0.0f, 0.0f},
                transformOffset + Vector2{0.0f, triangleWidth * squareRoot},
                transformOffset + Vector2{triangleWidth / squareRoot, triangleWidth * 0.5f * squareRoot},
                thumbColor);
        }
    });
    drawingContext.PushCommand(renderCommand);
}

} // namespace UI
} // namespace Pomdog
