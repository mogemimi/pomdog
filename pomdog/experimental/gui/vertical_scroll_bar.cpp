// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/vertical_scroll_bar.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/utility/string_helper.h"

namespace pomdog::gui {
namespace {

int CalculateSliderHeight(
    const VerticalScrollBar& scrollBar,
    double minimum_,
    double maximum_,
    double pageStep_)
{
    POMDOG_ASSERT(minimum_ < maximum_);
    POMDOG_ASSERT((maximum_ - minimum_) != 0);
    constexpr int sliderMinHeight = 10;
    const auto range = (maximum_ - minimum_) + pageStep_;

    POMDOG_ASSERT(pageStep_ <= range);
    const auto h = (pageStep_ / range) * scrollBar.getHeight();
    const auto sliderHeight = std::max(static_cast<int>(h), sliderMinHeight);

    return sliderHeight;
}

Rect2D
CalculateSliderRect(
    const VerticalScrollBar& scrollBar,
    const Point2D& position,
    double value_,
    double minimum_,
    double maximum_,
    double pageStep_)
{
    const auto sliderHeight = CalculateSliderHeight(scrollBar, minimum_, maximum_, pageStep_);

    const auto documentLength = scrollBar.getHeight() - sliderHeight;
    const auto ratio = (value_ - minimum_) / (maximum_ - minimum_);
    const auto controlPosition = static_cast<int>(documentLength * ratio);

    Rect2D sliderRect{
        position.x,
        position.y + controlPosition,
        scrollBar.getWidth(),
        sliderHeight};

    return sliderRect;
}

} // namespace

VerticalScrollBar::VerticalScrollBar(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    double minimumIn,
    double maximumIn)
    : Widget(dispatcher)
    , minimum_(minimumIn)
    , maximum_(maximumIn)
    , value_(0.0)
    , pageStep_(1.0)
    , isEnabled_(true)
    , isPointerEntered_(false)
{
    POMDOG_ASSERT(minimum_ < maximum_);
    POMDOG_ASSERT(value_ >= minimum_);
    POMDOG_ASSERT(value_ <= maximum_);

    setSize(10, 120);
}

double VerticalScrollBar::getValue() const noexcept
{
    return value_;
}

void VerticalScrollBar::setValue(double valueIn)
{
    if (value_ == valueIn) {
        return;
    }
    value_ = valueIn;
    ValueChanged(value_);
}

double VerticalScrollBar::getMinimum() const noexcept
{
    return minimum_;
}

void VerticalScrollBar::setMinimum(double minimumIn) noexcept
{
    minimum_ = minimumIn;
}

double VerticalScrollBar::getMaximum() const noexcept
{
    return maximum_;
}

void VerticalScrollBar::setMaximum(double maximumIn) noexcept
{
    maximum_ = maximumIn;
}

double VerticalScrollBar::getPageStep() const noexcept
{
    return pageStep_;
}

void VerticalScrollBar::setPageStep(double pageStepIn) noexcept
{
    pageStep_ = pageStepIn;
}

bool VerticalScrollBar::isEnabled() const
{
    return isEnabled_;
}

void VerticalScrollBar::setEnabled(bool isEnabledIn)
{
    isEnabled_ = isEnabledIn;
}

HorizontalAlignment VerticalScrollBar::getHorizontalAlignment() const noexcept
{
    return HorizontalAlignment::Right;
}

bool VerticalScrollBar::getSizeToFitContent() const noexcept
{
    return false;
}

void VerticalScrollBar::onEnter()
{
}

void VerticalScrollBar::onPointerEntered([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled_) {
        return;
    }

    isPointerEntered_ = true;

    ColorAnimation animation;
    animation.duration = 0.19f;
    animation.time = 0.0f;
    if (colorAnimation_) {
        const auto scaleFactor = colorAnimation_->time / colorAnimation_->duration;
        animation.time = animation.duration * (1.0f - scaleFactor);
    }

    colorAnimation_ = animation;
}

void VerticalScrollBar::onPointerExited([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isPointerEntered_ = false;

    ColorAnimation animation;
    animation.duration = 0.15f;
    animation.time = 0.0f;
    if (colorAnimation_) {
        const auto scaleFactor = colorAnimation_->time / colorAnimation_->duration;
        animation.time = animation.duration * (1.0f - scaleFactor);
    }

    colorAnimation_ = animation;
}

void VerticalScrollBar::onPointerPressed(const PointerPoint& pointerPoint)
{
    if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != PointerMouseEvent::LeftButtonPressed) {
        return;
    }

    if (!isEnabled_) {
        return;
    }

    POMDOG_ASSERT(getWidth() > 0);

    auto pointInView = UIHelper::projectToChildSpace(pointerPoint.Position, getGlobalPosition());

    auto sliderRect = CalculateSliderRect(*this, Point2D{0, 0}, value_, minimum_, maximum_, pageStep_);
    if (sliderRect.contains(pointInView)) {
        sliderGrabPosition_ = pointInView.y - sliderRect.y;
    }
    else {
        const auto sliderHeight = CalculateSliderHeight(*this, minimum_, maximum_, pageStep_);

        sliderGrabPosition_ = sliderHeight / 2;

        POMDOG_ASSERT((getHeight() - sliderHeight) > 0);
        const auto documentLength = (getHeight() - sliderHeight);
        const auto amount = static_cast<double>(pointInView.y - *sliderGrabPosition_) / documentLength;
        setValue(math::clamp(amount * (maximum_ - minimum_) + minimum_, minimum_, maximum_));
    }
}

void VerticalScrollBar::onPointerMoved(const PointerPoint& pointerPoint)
{
    if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != PointerMouseEvent::LeftButtonPressed) {
        return;
    }

    if (!isEnabled_) {
        return;
    }

    if (sliderGrabPosition_ == std::nullopt) {
        return;
    }

    auto pointInView = UIHelper::projectToChildSpace(pointerPoint.Position, getGlobalPosition());

    const auto sliderHeight = CalculateSliderHeight(*this, minimum_, maximum_, pageStep_);
    POMDOG_ASSERT((getHeight() - sliderHeight) > 0);

    const auto documentLength = getHeight() - sliderHeight;
    const auto amount = static_cast<double>(pointInView.y - *sliderGrabPosition_) / documentLength;
    setValue(math::clamp(amount * (maximum_ - minimum_) + minimum_, minimum_, maximum_));
}

void VerticalScrollBar::onPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    sliderGrabPosition_ = std::nullopt;
}

void VerticalScrollBar::updateAnimation(const Duration& frameDuration)
{
    if (!colorAnimation_) {
        return;
    }

    colorAnimation_->time += static_cast<float>(frameDuration.count());
    colorAnimation_->time = std::min(colorAnimation_->time, colorAnimation_->duration);

    if (colorAnimation_->time >= colorAnimation_->duration) {
        colorAnimation_ = std::nullopt;
    }
}

void VerticalScrollBar::draw(DrawingContext& drawingContext)
{
    POMDOG_ASSERT(value_ >= minimum_);
    POMDOG_ASSERT(value_ <= maximum_);

    const auto* colorScheme = drawingContext.getColorScheme();
    POMDOG_ASSERT(colorScheme != nullptr);

    auto thumbColor = colorScheme->ScrollBarSliderColorBase;
    if (colorAnimation_ != std::nullopt) {
        POMDOG_ASSERT(isEnabled_);
        auto startColor = colorScheme->ScrollBarSliderColorBase;
        auto targetColor = colorScheme->ScrollBarSliderColorHovered;
        if (!isPointerEntered_) {
            std::swap(startColor, targetColor);
        }
        const auto scaleFactor = colorAnimation_->time / colorAnimation_->duration;
        thumbColor = math::smoothstep(startColor, targetColor, scaleFactor);
    }
    else if (isPointerEntered_) {
        POMDOG_ASSERT(isEnabled_);
        POMDOG_ASSERT(colorAnimation_ == std::nullopt);
        thumbColor = colorScheme->ScrollBarSliderColorHovered;
    }
    else {
        POMDOG_ASSERT(isEnabled_);
        POMDOG_ASSERT(colorAnimation_ == std::nullopt);
        POMDOG_ASSERT(!isPointerEntered_);
        thumbColor = colorScheme->ScrollBarSliderColorBase;
    }

    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());

    auto primitiveBatch = drawingContext.getPrimitiveBatch();

    primitiveBatch->drawRectangle(
        Rect2D{globalPos.x, globalPos.y, getWidth(), getHeight()},
        colorScheme->ScrollBarBorderColor);

    primitiveBatch->drawRectangle(
        Rect2D{globalPos.x + 1, globalPos.y, getWidth() - 2, getHeight()},
        colorScheme->ScrollBarTrackColor);

    if (isEnabled_) {
        // NOTE: draw Thumb
        auto sliderRect = CalculateSliderRect(*this, globalPos, value_, minimum_, maximum_, pageStep_);
        primitiveBatch->drawRectangle(sliderRect, thumbColor);
    }

    drawingContext.flushPrimitiveBatch();
}

} // namespace pomdog::gui
