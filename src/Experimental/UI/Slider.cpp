// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/UI/Slider.hpp"
#include "Pomdog/Experimental/UI/DrawingContext.hpp"
#include "Pomdog/Experimental/UI/PointerPoint.hpp"
#include "Pomdog/Experimental/UI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/UI/UIHelper.hpp"
#include "Pomdog/Utility/StringHelper.hpp"

namespace Pomdog::UI {

Slider::Slider(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    double minimumIn,
    double maximumIn)
    : Slider(dispatcher, SliderColorScheme{}, minimumIn, maximumIn)
{
}

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
//    , borderColor(colorSchemeIn.BorderColor)
    , fillColor(colorSchemeIn.FillColor1)
    , trackColor(colorSchemeIn.TrackColor)
    , isDragging(false)
    , isEnabled(true)
    , isTextVisible(true)
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

void Slider::SetTextVisible(bool isTextVisibleIn)
{
    isTextVisible = isTextVisibleIn;
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

void Slider::OnPointerEntered([[maybe_unused]] const PointerPoint& pointerPoint)
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

void Slider::OnPointerExited([[maybe_unused]] const PointerPoint& pointerPoint)
{
    ColorAnimation animation;
    animation.duration = 0.15f;
    animation.startColor = fillColor;
    animation.targetColor = (isEnabled) ? colorScheme.FillColor1 : colorScheme.DisabledFillColor;

    colorAnimation = animation;
}

void Slider::OnPointerPressed([[maybe_unused]] const PointerPoint& pointerPoint)
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

void Slider::OnPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isDragging = false;
}

void Slider::UpdateAnimation(const Duration& frameDuration)
{
    if (!colorAnimation) {
        return;
    }

    colorAnimation->time += static_cast<float>(frameDuration.count());
    colorAnimation->time = std::min(colorAnimation->time, colorAnimation->duration);

    fillColor = Color::SmoothStep(colorAnimation->startColor, colorAnimation->targetColor,
        colorAnimation->time / colorAnimation->duration);

    if (colorAnimation->time >= colorAnimation->duration) {
        colorAnimation = std::nullopt;
    }
}

void Slider::Draw(DrawingContext& drawingContext)
{
    POMDOG_ASSERT(value >= minimum);
    POMDOG_ASSERT(value <= maximum);

    auto globalTransform = GetTransform() * drawingContext.Top();

    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    const auto sliderWidth2 = static_cast<float>(GetWidth() * ((value - minimum) / (maximum - minimum)));
    const auto controlPosition2 = static_cast<float>((GetWidth() - GetHeight()) * ((value - minimum) / (maximum - minimum)));

    const auto sliderHeight = static_cast<float>(GetHeight());
    const auto paddingY = (static_cast<float>(GetHeight()) - sliderHeight) * 0.5f;

    primitiveBatch->DrawRectangle(
        globalTransform,
        Vector2{0.0f, paddingY},
        static_cast<float>(GetWidth()),
        sliderHeight,
        trackColor);
    primitiveBatch->DrawRectangle(
        globalTransform,
        Vector2{0.0f, paddingY},
        sliderWidth2,
        sliderHeight,
        fillColor);

    const auto thumbSize = static_cast<float>(GetHeight());

    if (isEnabled && isDragging) {
        constexpr float pixel = 2.0f;

        auto pos = Vector2(controlPosition2 - pixel, -pixel);
        auto size = Vector2(thumbSize + 2 * pixel, thumbSize + 2 * pixel);

        primitiveBatch->DrawRectangle(
            globalTransform,
            pos,
            size.X,
            size.Y,
            colorScheme.FocusedThumbColor);
    }

    if (isEnabled) {
        auto pos = Vector2(controlPosition2, 0);
        primitiveBatch->DrawRectangle(
            globalTransform,
            pos,
            thumbSize,
            thumbSize,
            colorScheme.ThumbColor);
    }
    primitiveBatch->Flush();

    if (isTextVisible) {
        auto spriteBatch = drawingContext.GetSpriteBatch();
        auto spriteFont = drawingContext.GetFont(FontWeight::Normal, FontSize::Medium);
        auto text = StringHelper::Format("%5.3lf", value);

        constexpr float textMarginX = 2.0f;
        constexpr float textMarginY = 3.5f;
        Vector2 textPosition;
        textPosition.Y = globalTransform(2, 1) + GetHeight() * 0.5f + textMarginY;
        Vector2 originPivot;
        if (value < MathHelper::Lerp(minimum, maximum, 0.6)) {
            originPivot = Vector2{1.0f, 0.5f};
            textPosition.X = globalTransform(2, 0) + GetWidth() - textMarginX;
        }
        else {
            originPivot = Vector2{0.0f, 0.5f};
            textPosition.X = globalTransform(2, 0) + textMarginX;
        }
        spriteFont->Draw(*spriteBatch, text, textPosition, Color::Black, 0.0f, originPivot, 1.0f);
    }
}

} // namespace Pomdog::UI
