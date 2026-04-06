// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/check_box.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/experimental/tween/easing_helper.h"

namespace pomdog::gui {
namespace {

constexpr int checkBoxWidth = 20;
constexpr int checkBoxHeight = 20;

} // namespace

CheckBox::CheckBox(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , isOn_(true)
    , isEnabled_(true)
{
    setSize(checkBoxWidth, checkBoxHeight);
    setCursor(MouseCursor::PointingHand);
}

bool CheckBox::isOn() const
{
    return isOn_;
}

void CheckBox::setOn(bool isOnIn)
{
    isOn_ = isOnIn;
}

bool CheckBox::isEnabled() const
{
    return isEnabled_;
}

void CheckBox::setEnabled(bool isEnabledIn)
{
    isEnabled_ = isEnabledIn;
    if (isEnabled_) {
        setCursor(MouseCursor::PointingHand);
    }
    else {
        resetCursor();
    }
}

HorizontalAlignment CheckBox::getHorizontalAlignment() const noexcept
{
    return HorizontalAlignment::Right;
}

VerticalAlignment CheckBox::getVerticalAlignment() const noexcept
{
    return VerticalAlignment::Top;
}

void CheckBox::onEnter()
{
}

void CheckBox::onPointerPressed([[maybe_unused]] const PointerPoint& pointerPoint)
{
}

void CheckBox::onPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled_) {
        return;
    }

    isOn_ = !isOn_;
    Toggled(isOn_);
}

void CheckBox::draw(DrawingContext& drawingContext)
{
    const auto* colorScheme = drawingContext.getColorScheme();
    POMDOG_ASSERT(colorScheme != nullptr);

    auto innerColor = colorScheme->CheckBoxRectColorOff;
    auto checkColor = colorScheme->CheckBoxCheckMarkColorBase;

    if (!isEnabled_) {
        innerColor = colorScheme->CheckBoxRectColorDisabled;
        checkColor = colorScheme->CheckBoxCheckMarkColorDisabled;
    }
    else if (isOn_) {
        POMDOG_ASSERT(isEnabled_);
        innerColor = colorScheme->CheckBoxRectColorOn;
    }

    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());

    constexpr int iconPixelSize = 32;
    constexpr int checkIconPixelSize = 14;
    const auto transformOffset = math::toVector2(globalPos);

    drawingContext.drawIcon(
        "ios-square.svg",
        transformOffset + Vector2{-1.0f, -1.0f},
        Color{93, 90, 91, 255},
        0.0f,
        Vector2::createZero(),
        static_cast<float>(checkBoxWidth + 2) / iconPixelSize);

    drawingContext.drawIcon(
        "ios-square.svg",
        transformOffset,
        innerColor,
        0.0f,
        Vector2::createZero(),
        static_cast<float>(checkBoxWidth) / iconPixelSize);

    if (isOn_) {
        drawingContext.drawIcon(
            "md-checkmark.svg",
            transformOffset + Vector2{3.0f, 3.5f},
            checkColor,
            0.0f,
            Vector2::createZero(),
            static_cast<float>(checkIconPixelSize) / iconPixelSize);
    }
}

} // namespace pomdog::gui
