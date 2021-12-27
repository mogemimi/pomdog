// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/disclosure_triangle_button.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/math/math.h"

namespace pomdog::gui {

DisclosureTriangleButton::DisclosureTriangleButton(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , isOn(true)
    , isEnabled(true)
{
    SetSize(8, 8);
    SetCursor(MouseCursor::PointingHand);
}

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
    return HorizontalAlignment::Left;
}

VerticalAlignment DisclosureTriangleButton::GetVerticalAlignment() const noexcept
{
    return VerticalAlignment::Top;
}

void DisclosureTriangleButton::OnEnter()
{
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
    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());
    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    Color thumbColor;
    if (isOn) {
        thumbColor = Color{27, 161, 226, 255};
    }
    else {
        thumbColor = Color{162, 160, 161, 255};
    }

    const auto transformOffset = math::ToVector2(globalPos);

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

} // namespace pomdog::gui
