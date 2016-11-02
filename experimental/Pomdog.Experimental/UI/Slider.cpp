// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Slider.hpp"
#include "DrawingContext.hpp"
#include "PointerPoint.hpp"
#include "UIHelper.hpp"
#include "UIEventDispatcher.hpp"

namespace Pomdog {
namespace UI {

Slider::Slider(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    double minimumIn,
    double maximumIn)
    : Slider(dispatcher, SliderColorScheme{}, minimumIn, maximumIn)
{}

Slider::Slider(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    const SliderColorScheme& colorSchemeIn,
    double minimumIn,
    double maximumIn)
    : UIElement(dispatcher)
    , minimum(minimumIn)
    , maximum(maximumIn)
    , value(minimumIn)
    , colorScheme(colorSchemeIn)
    , borderColor(colorSchemeIn.BorderColor)
    , fillColor(colorSchemeIn.FillColor1)
    , trackColor(colorSchemeIn.TrackColor)
    , isDragging(false)
    , isEnabled(true)
{
    POMDOG_ASSERT(minimum < maximum);
    POMDOG_ASSERT(value >= minimum);
    POMDOG_ASSERT(value <= maximum);

    SetSize(120, 12);
    SetCursor(MouseCursor::PointingHand);
}

// MARK: - Properties

void Slider::SetValue(double valueIn)
{
    if (value == valueIn) {
        return;
    }
    this->value = valueIn;
    ValueChanged(this->value);
}

double Slider::GetValue() const
{
    return value;
}

double Slider::GetMinimum() const
{
    return minimum;
}

double Slider::GetMaximum() const
{
    return maximum;
}

bool Slider::IsEnabled() const
{
    return isEnabled;
}

void Slider::SetEnabled(bool isEnabledIn)
{
    this->isEnabled = isEnabledIn;
    if (isEnabled) {
        fillColor = colorScheme.FillColor1;
    }
    else {
        fillColor = colorScheme.DisabledFillColor;
    }
}

VerticalAlignment Slider::GetVerticalAlignment() const noexcept
{
    return UI::VerticalAlignment::Top;
}

bool Slider::SizeToFitContent() const
{
    return false;
}

// MARK: - Events

void Slider::OnEnter()
{
    auto dispatcher = GetDispatcher();
    connection = dispatcher->Connect(shared_from_this());
}

void Slider::OnPointerEntered(const PointerPoint& pointerPoint)
{
    if (!isEnabled) {
        return;
    }

    ColorAnimation animation;
    animation.duration = 0.19f;
    animation.startColor = fillColor;
    animation.targetColor = colorScheme.FillColor2;

    colorAnimation = animation;
}

void Slider::OnPointerExited(const PointerPoint& pointerPoint)
{
    ColorAnimation animation;
    animation.duration = 0.15f;
    animation.startColor = fillColor;
    animation.targetColor = (isEnabled) ?
        colorScheme.FillColor1: colorScheme.DisabledFillColor;

    colorAnimation = animation;
}

void Slider::OnPointerPressed(const PointerPoint& pointerPoint)
{
    if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != UI::PointerMouseEvent::LeftButtonPressed) {
        return;
    }

    if (!isEnabled) {
        return;
    }

    POMDOG_ASSERT(GetWidth() > 0);

    // NOTE: float thumbOffset = thumbWidth / 2
    constexpr float thumbOffset = 5;

    auto pointInView = UIHelper::ConvertToChildSpace(pointerPoint.Position, GetGlobalTransform());
    auto amount = (pointInView.X - thumbOffset / 2) / (GetWidth() - 2 * thumbOffset);
    SetValue(MathHelper::Clamp(amount * (maximum - minimum) + minimum, minimum, maximum));
    isDragging = true;
}

void Slider::OnPointerMoved(const PointerPoint& pointerPoint)
{
    if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != UI::PointerMouseEvent::LeftButtonPressed) {
        return;
    }

    if (!isEnabled) {
        return;
    }

    POMDOG_ASSERT(GetWidth() > 0);

    // NOTE: float thumbOffset = thumbWidth / 2
    constexpr float thumbOffset = 5;

    auto pointInView = UIHelper::ConvertToChildSpace(pointerPoint.Position, GetGlobalTransform());
    auto amount = (pointInView.X - thumbOffset / 2) / (GetWidth() - 2 * thumbOffset);
    SetValue(MathHelper::Clamp(amount * (maximum - minimum) + minimum, minimum, maximum));
}

void Slider::OnPointerReleased(const PointerPoint& pointerPoint)
{
    isDragging = false;
}

void Slider::UpdateAnimation(const Duration& frameDuration)
{
    if (!colorAnimation) {
        return;
    }

    colorAnimation->time += frameDuration.count();
    colorAnimation->time = std::min(colorAnimation->time, colorAnimation->duration);

    fillColor = Color::SmoothStep(colorAnimation->startColor, colorAnimation->targetColor,
        colorAnimation->time / colorAnimation->duration);

    if (colorAnimation->time >= colorAnimation->duration) {
        colorAnimation = Pomdog::NullOpt;
    }
}

void Slider::OnRenderSizeChanged(int widthIn, int heightIn)
{
    SetSize(widthIn, heightIn);
}

void Slider::Draw(DrawingContext & drawingContext)
{
    //MathHelper::Clamp(value, minimum, maximum);

    POMDOG_ASSERT(value >= minimum);
    POMDOG_ASSERT(value <= maximum);

    auto sliderWidth2 = GetWidth() * ((value - minimum) / (maximum - minimum));
    auto controlPosition2 = (GetWidth() - GetHeight()) * ((value - minimum) / (maximum - minimum));

    auto transform = GetTransform() * drawingContext.Top();

    drawingContext.DrawString(transform * Matrix3x2::CreateTranslation(Vector2(GetWidth() + 5, -2.5f)),
        Color::White, FontWeight::Normal, FontSize::Medium, StringHelper::Format("%5.3lf", value));
    drawingContext.DrawRectangle(transform, trackColor, Rectangle(0, 0, GetWidth(), GetHeight()));
    drawingContext.DrawRectangle(transform, fillColor, Rectangle(0, 0, sliderWidth2, GetHeight()));

    if (isEnabled && isDragging) {
        constexpr float pixel = 2.0f;

        auto pos = Vector2(controlPosition2 - pixel, -pixel);
        auto size = Vector2(GetHeight() + 2 * pixel, GetHeight() + 2 * pixel);

        drawingContext.DrawRectangle(transform, colorScheme.FocusedThumbColor,
            Rectangle(pos.X, pos.Y, size.X, size.Y));
    }

    if (isEnabled) {
        auto pos = Vector2(controlPosition2, 0);
        drawingContext.DrawRectangle(transform, colorScheme.ThumbColor,
            Rectangle(pos.X, pos.Y, GetHeight(), GetHeight()));
    }
}

} // namespace UI
} // namespace Pomdog
