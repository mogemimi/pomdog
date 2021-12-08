// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/check_box.hpp"
#include "pomdog/experimental/gui/drawing_context.hpp"
#include "pomdog/experimental/gui/pointer_point.hpp"
#include "pomdog/experimental/gui/ui_event_dispatcher.hpp"
#include "pomdog/experimental/gui/ui_helper.hpp"
#include "pomdog/experimental/tween/easing_helper.hpp"

namespace pomdog::gui {
namespace {

constexpr int checkBoxWidth = 20;
constexpr int checkBoxHeight = 20;

} // namespace

CheckBox::CheckBox(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , isOn(true)
    , isEnabled(true)
{
    SetSize(checkBoxWidth, checkBoxHeight);
    SetCursor(MouseCursor::PointingHand);
}

bool CheckBox::IsOn() const
{
    return isOn;
}

void CheckBox::SetOn(bool isOnIn)
{
    this->isOn = isOnIn;
}

bool CheckBox::IsEnabled() const
{
    return isEnabled;
}

void CheckBox::SetEnabled(bool isEnabledIn)
{
    this->isEnabled = isEnabledIn;
    if (isEnabled) {
        SetCursor(MouseCursor::PointingHand);
    }
    else {
        ResetCursor();
    }
}

HorizontalAlignment CheckBox::GetHorizontalAlignment() const noexcept
{
    return HorizontalAlignment::Right;
}

VerticalAlignment CheckBox::GetVerticalAlignment() const noexcept
{
    return VerticalAlignment::Top;
}

void CheckBox::OnEnter()
{
}

void CheckBox::OnPointerPressed([[maybe_unused]] const PointerPoint& pointerPoint)
{
}

void CheckBox::OnPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }

    isOn = !isOn;
    Toggled(isOn);
}

void CheckBox::Draw(DrawingContext& drawingContext)
{
    const auto* colorScheme = drawingContext.GetColorScheme();
    POMDOG_ASSERT(colorScheme != nullptr);

    auto innerColor = colorScheme->CheckBoxRectColorOff;
    auto checkColor = colorScheme->CheckBoxCheckMarkColorBase;

    if (!isEnabled) {
        innerColor = colorScheme->CheckBoxRectColorDisabled;
        checkColor = colorScheme->CheckBoxCheckMarkColorDisabled;
    }
    else if (isOn) {
        POMDOG_ASSERT(isEnabled);
        innerColor = colorScheme->CheckBoxRectColorOn;
    }

    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());

    constexpr int iconPixelSize = 32;
    constexpr int checkIconPixelSize = 14;
    const auto transformOffset = math::ToVector2(globalPos);

    drawingContext.DrawIcon(
        "ionicons/ios-square.svg",
        transformOffset + Vector2{-1.0f, -1.0f},
        Color{93, 90, 91, 255},
        0.0f,
        Vector2::Zero,
        static_cast<float>(checkBoxWidth + 2) / iconPixelSize);

    drawingContext.DrawIcon(
        "ionicons/ios-square.svg",
        transformOffset,
        innerColor,
        0.0f,
        Vector2::Zero,
        static_cast<float>(checkBoxWidth) / iconPixelSize);

    if (isOn) {
        drawingContext.DrawIcon(
            "ionicons/md-checkmark.svg",
            transformOffset + Vector2{3.0f, 3.5f},
            checkColor,
            0.0f,
            Vector2::Zero,
            static_cast<float>(checkIconPixelSize) / iconPixelSize);
    }
}

} // namespace pomdog::gui
