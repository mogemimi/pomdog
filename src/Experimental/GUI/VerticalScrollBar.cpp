// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/GUI/VerticalScrollBar.hpp"
#include "Pomdog/Experimental/GUI/DrawingContext.hpp"
#include "Pomdog/Experimental/GUI/PointerPoint.hpp"
#include "Pomdog/Experimental/GUI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/GUI/UIHelper.hpp"
#include "Pomdog/Math/Math.hpp"
#include "Pomdog/Utility/StringHelper.hpp"

namespace Pomdog::GUI {
namespace {

int CalculateSliderHeight(
    const VerticalScrollBar& scrollBar,
    double minimum,
    double maximum,
    double pageStep)
{
    POMDOG_ASSERT(minimum < maximum);
    POMDOG_ASSERT((maximum - minimum) != 0);
    constexpr int sliderMinHeight = 10;
    const auto range = (maximum - minimum) + pageStep;

    POMDOG_ASSERT(pageStep <= range);
    const auto h = (pageStep / range) * scrollBar.GetHeight();
    const auto sliderHeight = std::max(static_cast<int>(h), sliderMinHeight);

    return sliderHeight;
}

Rectangle
CalculateSliderRect(
    const VerticalScrollBar& scrollBar,
    const Point2D& position,
    double value,
    double minimum,
    double maximum,
    double pageStep)
{
    const auto sliderHeight = CalculateSliderHeight(scrollBar, minimum, maximum, pageStep);

    const auto documentLength = scrollBar.GetHeight() - sliderHeight;
    const auto ratio = (value - minimum) / (maximum - minimum);
    const auto controlPosition = static_cast<int>(documentLength * ratio);

    Rectangle sliderRect{
        position.X,
        position.Y + controlPosition,
        scrollBar.GetWidth(),
        sliderHeight};

    return sliderRect;
}

} // namespace

VerticalScrollBar::VerticalScrollBar(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    double minimumIn,
    double maximumIn)
    : Widget(dispatcher)
    , minimum(minimumIn)
    , maximum(maximumIn)
    , value(0.0)
    , pageStep(1.0)
    , isEnabled(true)
    , isPointerEntered(false)
{
    POMDOG_ASSERT(minimum < maximum);
    POMDOG_ASSERT(value >= minimum);
    POMDOG_ASSERT(value <= maximum);

    SetSize(10, 120);
}

double VerticalScrollBar::GetValue() const noexcept
{
    return value;
}

void VerticalScrollBar::SetValue(double valueIn)
{
    if (value == valueIn) {
        return;
    }
    this->value = valueIn;
    ValueChanged(this->value);
}

double VerticalScrollBar::GetMinimum() const noexcept
{
    return minimum;
}

void VerticalScrollBar::SetMinimum(double minimumIn) noexcept
{
    this->minimum = minimumIn;
}

double VerticalScrollBar::GetMaximum() const noexcept
{
    return maximum;
}

void VerticalScrollBar::SetMaximum(double maximumIn) noexcept
{
    this->maximum = maximumIn;
}

double VerticalScrollBar::GetPageStep() const noexcept
{
    return pageStep;
}

void VerticalScrollBar::SetPageStep(double pageStepIn) noexcept
{
    this->pageStep = pageStepIn;
}

bool VerticalScrollBar::IsEnabled() const
{
    return isEnabled;
}

void VerticalScrollBar::SetEnabled(bool isEnabledIn)
{
    this->isEnabled = isEnabledIn;
}

HorizontalAlignment VerticalScrollBar::GetHorizontalAlignment() const noexcept
{
    return HorizontalAlignment::Right;
}

bool VerticalScrollBar::GetSizeToFitContent() const noexcept
{
    return false;
}

void VerticalScrollBar::OnEnter()
{
}

void VerticalScrollBar::OnPointerEntered([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }

    isPointerEntered = true;

    ColorAnimation animation;
    animation.duration = 0.19f;
    animation.time = 0.0f;
    if (colorAnimation) {
        const auto scaleFactor = colorAnimation->time / colorAnimation->duration;
        animation.time = animation.duration * (1.0f - scaleFactor);
    }

    colorAnimation = animation;
}

void VerticalScrollBar::OnPointerExited([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isPointerEntered = false;

    ColorAnimation animation;
    animation.duration = 0.15f;
    animation.time = 0.0f;
    if (colorAnimation) {
        const auto scaleFactor = colorAnimation->time / colorAnimation->duration;
        animation.time = animation.duration * (1.0f - scaleFactor);
    }

    colorAnimation = animation;
}

void VerticalScrollBar::OnPointerPressed(const PointerPoint& pointerPoint)
{
    if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != PointerMouseEvent::LeftButtonPressed) {
        return;
    }

    if (!isEnabled) {
        return;
    }

    POMDOG_ASSERT(GetWidth() > 0);

    auto pointInView = UIHelper::ProjectToChildSpace(pointerPoint.Position, GetGlobalPosition());

    auto sliderRect = CalculateSliderRect(*this, Point2D{0, 0}, value, minimum, maximum, pageStep);
    if (sliderRect.Contains(pointInView)) {
        sliderGrabPosition = pointInView.Y - sliderRect.Y;
    }
    else {
        const auto sliderHeight = CalculateSliderHeight(*this, minimum, maximum, pageStep);

        sliderGrabPosition = sliderHeight / 2;

        POMDOG_ASSERT((GetHeight() - sliderHeight) > 0);
        const auto documentLength = (GetHeight() - sliderHeight);
        const auto amount = static_cast<double>(pointInView.Y - *sliderGrabPosition) / documentLength;
        SetValue(Math::Clamp(amount * (maximum - minimum) + minimum, minimum, maximum));
    }
}

void VerticalScrollBar::OnPointerMoved(const PointerPoint& pointerPoint)
{
    if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != PointerMouseEvent::LeftButtonPressed) {
        return;
    }

    if (!isEnabled) {
        return;
    }

    if (sliderGrabPosition == std::nullopt) {
        return;
    }

    auto pointInView = UIHelper::ProjectToChildSpace(pointerPoint.Position, GetGlobalPosition());

    const auto sliderHeight = CalculateSliderHeight(*this, minimum, maximum, pageStep);
    POMDOG_ASSERT((GetHeight() - sliderHeight) > 0);

    const auto documentLength = GetHeight() - sliderHeight;
    const auto amount = static_cast<double>(pointInView.Y - *sliderGrabPosition) / documentLength;
    SetValue(Math::Clamp(amount * (maximum - minimum) + minimum, minimum, maximum));
}

void VerticalScrollBar::OnPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    sliderGrabPosition = std::nullopt;
}

void VerticalScrollBar::UpdateAnimation(const Duration& frameDuration)
{
    if (!colorAnimation) {
        return;
    }

    colorAnimation->time += static_cast<float>(frameDuration.count());
    colorAnimation->time = std::min(colorAnimation->time, colorAnimation->duration);

    if (colorAnimation->time >= colorAnimation->duration) {
        colorAnimation = std::nullopt;
    }
}

void VerticalScrollBar::Draw(DrawingContext& drawingContext)
{
    POMDOG_ASSERT(value >= minimum);
    POMDOG_ASSERT(value <= maximum);

    const auto* colorScheme = drawingContext.GetColorScheme();
    POMDOG_ASSERT(colorScheme != nullptr);

    auto thumbColor = colorScheme->ScrollBarSliderColorBase;
    if (colorAnimation != std::nullopt) {
        POMDOG_ASSERT(isEnabled);
        auto startColor = colorScheme->ScrollBarSliderColorBase;
        auto targetColor = colorScheme->ScrollBarSliderColorHovered;
        if (!isPointerEntered) {
            std::swap(startColor, targetColor);
        }
        const auto scaleFactor = colorAnimation->time / colorAnimation->duration;
        thumbColor = Color::SmoothStep(startColor, targetColor, scaleFactor);
    }
    else if (isPointerEntered) {
        POMDOG_ASSERT(isEnabled);
        POMDOG_ASSERT(colorAnimation == std::nullopt);
        thumbColor = colorScheme->ScrollBarSliderColorHovered;
    }
    else {
        POMDOG_ASSERT(isEnabled);
        POMDOG_ASSERT(colorAnimation == std::nullopt);
        POMDOG_ASSERT(!isPointerEntered);
        thumbColor = colorScheme->ScrollBarSliderColorBase;
    }

    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());

    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    primitiveBatch->DrawRectangle(
        Rectangle{globalPos.X, globalPos.Y, GetWidth(), GetHeight()},
        colorScheme->ScrollBarBorderColor);

    primitiveBatch->DrawRectangle(
        Rectangle{globalPos.X + 1, globalPos.Y, GetWidth() - 2, GetHeight()},
        colorScheme->ScrollBarTrackColor);

    if (isEnabled) {
        // NOTE: Draw Thumb
        auto sliderRect = CalculateSliderRect(*this, globalPos, value, minimum, maximum, pageStep);
        primitiveBatch->DrawRectangle(sliderRect, thumbColor);
    }

    primitiveBatch->Flush();
}

} // namespace Pomdog::GUI
