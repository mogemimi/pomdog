// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/toggle_switch.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/experimental/tween/easing_helper.h"

namespace pomdog::gui {
namespace {

constexpr float thumbRadius = 7.0f;
constexpr float thumbPadding = 2.0f;
constexpr int segments = 12;

} // namespace

ToggleSwitch::ToggleSwitch(const std::shared_ptr<UIEventDispatcher>& dispatcher)
    : Widget(dispatcher)
    , toggleAnimation_(ToggleAnimation::Stopped)
    , animationDuration_(0.0f)
    , isOn_(true)
    , isEnabled_(true)
{
    setSize(48, static_cast<int>(thumbRadius) * 2);
    setCursor(MouseCursor::PointingHand);
}

bool ToggleSwitch::isOn() const
{
    return isOn_;
}

void ToggleSwitch::setOn(bool isOnIn)
{
    isOn_ = isOnIn;
}

bool ToggleSwitch::isEnabled() const
{
    return isEnabled_;
}

void ToggleSwitch::setEnabled(bool isEnabledIn)
{
    isEnabled_ = isEnabledIn;
    if (isEnabled_) {
        setCursor(MouseCursor::PointingHand);
    }
    else {
        resetCursor();
    }
}

HorizontalAlignment ToggleSwitch::getHorizontalAlignment() const noexcept
{
    return HorizontalAlignment::Right;
}

VerticalAlignment ToggleSwitch::getVerticalAlignment() const noexcept
{
    return VerticalAlignment::Top;
}

void ToggleSwitch::onEnter()
{
}

void ToggleSwitch::onPointerPressed([[maybe_unused]] const PointerPoint& pointerPoint)
{
}

void ToggleSwitch::onPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled_) {
        return;
    }

    isOn_ = !isOn_;

    if (isOn_) {
        toggleAnimation_ = ToggleAnimation::OffToOn;
    }
    else {
        toggleAnimation_ = ToggleAnimation::OnToOff;
    }
    animationDuration_ = 0.0f;

    Toggled(isOn_);
}

void ToggleSwitch::updateAnimation(const Duration& frameDuration)
{
    if (toggleAnimation_ == ToggleAnimation::Stopped) {
        return;
    }

    constexpr double animationSpeed = 7.0;
    animationDuration_ += static_cast<float>(frameDuration.count() * animationSpeed);
    if (animationDuration_ > 1.0f) {
        toggleAnimation_ = ToggleAnimation::Stopped;
        animationDuration_ = 0.0f;
    }
}

void ToggleSwitch::draw(DrawingContext& drawingContext)
{
    const Color toggleOnInnerColor = {162, 122, 16, 255};
    const Color toggleOnThumbColor = {252, 252, 250, 255};
    const Color toggleOffInnerColor = {160, 160, 160, 255};
    const Color toggleOffThumbColor = {81, 81, 81, 255};
    const Color disabledInnerColor = {81, 81, 81, 255};
    const Color disabledThumbColor = {137, 137, 137, 255};

    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());
    auto primitiveBatch = drawingContext.getPrimitiveBatch();

    auto innerColor = Color{109, 109, 109, 255};
    auto thumbColor = Color::createWhite();

    if (!isEnabled_) {
        innerColor = disabledInnerColor;
        thumbColor = disabledThumbColor;
    }

    if (isEnabled_) {
        if (isOn_) {
            innerColor = toggleOnInnerColor;
            thumbColor = toggleOnThumbColor;
        }
        else {
            innerColor = toggleOffInnerColor;
            thumbColor = toggleOffThumbColor;
        }
    }

    const auto transformOffset = math::toVector2(globalPos);

    primitiveBatch->drawArc(
        transformOffset + Vector2{
                              static_cast<float>(getWidth()) - (thumbRadius + thumbPadding),
                              (thumbRadius + thumbPadding)},
        thumbRadius + thumbPadding, math::toRadian(270.0f), math::toRadian(180.0f), segments, innerColor);

    primitiveBatch->drawArc(
        transformOffset + Vector2{
                              (thumbRadius + thumbPadding),
                              (thumbRadius + thumbPadding)},
        thumbRadius + thumbPadding, math::toRadian(90.0f), math::toRadian(180.0f), segments, innerColor);

    primitiveBatch->drawRectangle(
        Matrix3x2::createIdentity(),
        Vector2{(thumbRadius + thumbPadding), 0.0f} + math::toVector2(globalPos),
        getWidth() - (thumbRadius + thumbPadding) * 2.0f,
        (thumbRadius + thumbPadding) * 2.0f,
        innerColor);

    const auto onPos = Vector2{
        static_cast<float>(getWidth()) - (thumbRadius + thumbPadding),
        (thumbRadius + thumbPadding)};
    const auto offPos = Vector2{
        (thumbRadius + thumbPadding),
        (thumbRadius + thumbPadding)};

    Vector2 circlePos;
    if (isOn_) {
        circlePos = onPos;
    }
    else {
        circlePos = offPos;
    }
    switch (toggleAnimation_) {
    case ToggleAnimation::OnToOff:
        circlePos = math::lerp(offPos, onPos, 1.0f - Easings::EaseSine::InOut(animationDuration_));
        break;
    case ToggleAnimation::OffToOn:
        circlePos = math::lerp(offPos, onPos, Easings::EaseSine::InOut(animationDuration_));
        break;
    case ToggleAnimation::Stopped:
        break;
    }

    primitiveBatch->drawCircle(
        transformOffset + circlePos,
        thumbRadius,
        segments,
        thumbColor);
    primitiveBatch->flush();

    auto spriteBatch = drawingContext.getSpriteBatch();
    auto spriteFont = drawingContext.getFont(FontWeight::Bold, FontSize::Small);

    if (isOn_) {
        auto textPosition = math::toVector2(globalPos) + Vector2{thumbPadding, thumbPadding} + Vector2{9.0f, 3.0f};
        spriteFont->draw(*spriteBatch, "ON", textPosition, toggleOnThumbColor);
    }
    else {
        auto textPosition = math::toVector2(globalPos) + Vector2{static_cast<float>(getWidth()) - (thumbRadius + thumbPadding), thumbPadding} + Vector2{-19.0f, 3.0f};
        spriteFont->draw(*spriteBatch, "OFF", textPosition, Color{250, 250, 250, 255});
    }

    drawingContext.flushSpriteBatch();
}

} // namespace pomdog::gui
