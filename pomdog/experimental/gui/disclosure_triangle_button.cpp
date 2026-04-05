// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/disclosure_triangle_button.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/math/math_functions.h"

namespace pomdog::gui {

DisclosureTriangleButton::DisclosureTriangleButton(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , isOn_(true)
    , isEnabled_(true)
{
    setSize(8, 8);
    setCursor(MouseCursor::PointingHand);
}

bool DisclosureTriangleButton::isOn() const
{
    return isOn_;
}

void DisclosureTriangleButton::setOn(bool isOnIn)
{
    isOn_ = isOnIn;
}

bool DisclosureTriangleButton::isEnabled() const
{
    return isEnabled_;
}

void DisclosureTriangleButton::setEnabled(bool isEnabledIn)
{
    isEnabled_ = isEnabledIn;
    if (isEnabled_) {
        setCursor(MouseCursor::PointingHand);
    }
    else {
        resetCursor();
    }
}

HorizontalAlignment DisclosureTriangleButton::getHorizontalAlignment() const noexcept
{
    return HorizontalAlignment::Left;
}

VerticalAlignment DisclosureTriangleButton::getVerticalAlignment() const noexcept
{
    return VerticalAlignment::Top;
}

void DisclosureTriangleButton::onEnter()
{
}

void DisclosureTriangleButton::onPointerPressed([[maybe_unused]] const PointerPoint& pointerPoint)
{
}

void DisclosureTriangleButton::onPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled_) {
        return;
    }
    isOn_ = !isOn_;
    Toggled(isOn_);
}

void DisclosureTriangleButton::draw(DrawingContext& drawingContext)
{
    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());
    auto primitiveBatch = drawingContext.getPrimitiveBatch();

    Color thumbColor;
    if (isOn_) {
        thumbColor = Color{27, 161, 226, 255};
    }
    else {
        thumbColor = Color{162, 160, 161, 255};
    }

    const auto transformOffset = math::toVector2(globalPos);

    constexpr float triangleWidth = 7.0f;
    constexpr float triangleHeight = triangleWidth * 1.73205f * 0.5f;
    if (isOn_) {
        primitiveBatch->drawTriangle(
            transformOffset + Vector2{triangleWidth * 0.5f, 0.0f},
            transformOffset + Vector2{0.0f, triangleHeight},
            transformOffset + Vector2{triangleWidth, triangleHeight},
            thumbColor);
    }
    else {
        primitiveBatch->drawTriangle(
            transformOffset + Vector2{0.0f, 0.0f},
            transformOffset + Vector2{0.0f, triangleWidth},
            transformOffset + Vector2{triangleHeight, triangleWidth * 0.5f},
            thumbColor);
    }
    primitiveBatch->flush();
}

} // namespace pomdog::gui
