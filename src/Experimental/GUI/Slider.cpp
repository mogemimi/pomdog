// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/GUI/Slider.hpp"
#include "Pomdog/Experimental/GUI/DrawingContext.hpp"
#include "Pomdog/Experimental/GUI/PointerPoint.hpp"
#include "Pomdog/Experimental/GUI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/GUI/UIHelper.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Utility/StringHelper.hpp"

namespace Pomdog::GUI {

Slider::Slider(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    double minimumIn,
    double maximumIn)
    : Widget(dispatcher)
    , minimum(minimumIn)
    , maximum(maximumIn)
    , value(minimumIn)
    , isDragging(false)
    , isEnabled(true)
    , isTextVisible(true)
    , isThumbVisible(false)
    , isPointerEntered(false)
{
    POMDOG_ASSERT(minimum < maximum);
    POMDOG_ASSERT(value >= minimum);
    POMDOG_ASSERT(value <= maximum);

    SetSize(120, 18);
    SetCursor(MouseCursor::ResizeHorizontal);
}

void Slider::SetValue(double valueIn)
{
    if (value == valueIn) {
        return;
    }
    this->value = valueIn;
    ValueChanged(this->value);
}

double Slider::GetValue() const noexcept
{
    return value;
}

double Slider::GetMinimum() const noexcept
{
    return minimum;
}

void Slider::SetMinimum(double minimumIn)
{
    if (minimum == minimumIn) {
        return;
    }
    minimum = minimumIn;
    if (minimum > value) {
        value = minimum;
        ValueChanged(value);
    }
}

void Slider::SetMaximum(double maximumIn)
{
    if (maximum == maximumIn) {
        return;
    }
    maximum = maximumIn;
    if (maximum < value) {
        value = maximum;
        ValueChanged(value);
    }
}

double Slider::GetMaximum() const noexcept
{
    return maximum;
}

bool Slider::IsEnabled() const noexcept
{
    return isEnabled;
}

void Slider::SetEnabled(bool enabledIn) noexcept
{
    this->isEnabled = enabledIn;
}

void Slider::SetTextVisible(bool isTextVisibleIn)
{
    isTextVisible = isTextVisibleIn;
}

void Slider::SetThumbVisible(bool isThumbVisibleIn)
{
    isThumbVisible = isThumbVisibleIn;
}

VerticalAlignment Slider::GetVerticalAlignment() const noexcept
{
    return VerticalAlignment::Top;
}

bool Slider::GetSizeToFitContent() const noexcept
{
    return false;
}

void Slider::OnEnter()
{
}

void Slider::OnPointerEntered([[maybe_unused]] const PointerPoint& pointerPoint)
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

void Slider::OnPointerExited([[maybe_unused]] const PointerPoint& pointerPoint)
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

void Slider::OnPointerPressed([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != PointerMouseEvent::LeftButtonPressed) {
        return;
    }

    if (!isEnabled) {
        return;
    }

    POMDOG_ASSERT(GetWidth() > 0);

    // NOTE: float thumbOffset = thumbWidth / 2
    constexpr float thumbOffset = 5;

    auto pointInView = UIHelper::ProjectToChildSpace(pointerPoint.Position, GetGlobalPosition());
    auto amount = (pointInView.X - thumbOffset / 2) / (GetWidth() - 2 * thumbOffset);
    SetValue(MathHelper::Clamp(amount * (maximum - minimum) + minimum, minimum, maximum));
    isDragging = true;
}

void Slider::OnPointerMoved(const PointerPoint& pointerPoint)
{
    if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != PointerMouseEvent::LeftButtonPressed) {
        return;
    }

    if (!isEnabled) {
        return;
    }

    POMDOG_ASSERT(GetWidth() > 0);

    // NOTE: float thumbOffset = thumbWidth / 2
    constexpr float thumbOffset = 5;

    auto pointInView = UIHelper::ProjectToChildSpace(pointerPoint.Position, GetGlobalPosition());
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

    if (colorAnimation->time >= colorAnimation->duration) {
        colorAnimation = std::nullopt;
    }
}

void Slider::Draw(DrawingContext& drawingContext)
{
    POMDOG_ASSERT(value >= minimum);
    POMDOG_ASSERT(value <= maximum);

    const auto* colorScheme = drawingContext.GetColorScheme();
    POMDOG_ASSERT(colorScheme != nullptr);

    auto fillColor = colorScheme->SliderFillColorBase;
    auto trackColor = colorScheme->SliderTrackColorBase;

    if (!isEnabled) {
        fillColor = colorScheme->SliderFillColorDisabled;
    }
    else if (colorAnimation != std::nullopt) {
        POMDOG_ASSERT(isEnabled);
        auto startColor = colorScheme->SliderFillColorBase;
        auto targetColor = colorScheme->SliderFillColorHovered;
        if (!isPointerEntered) {
            std::swap(startColor, targetColor);
        }
        const auto scaleFactor = colorAnimation->time / colorAnimation->duration;
        fillColor = Color::SmoothStep(startColor, targetColor, scaleFactor);
    }
    else if (isPointerEntered) {
        POMDOG_ASSERT(isEnabled);
        POMDOG_ASSERT(colorAnimation == std::nullopt);
        fillColor = colorScheme->SliderFillColorHovered;
    }
    else {
        POMDOG_ASSERT(isEnabled);
        POMDOG_ASSERT(colorAnimation == std::nullopt);
        POMDOG_ASSERT(!isPointerEntered);
        fillColor = colorScheme->SliderFillColorBase;
    }

    if (isPointerEntered) {
        trackColor = colorScheme->SliderTrackColorHovered;
    }

    auto globalPos = UIHelper::ProjectToWorldSpace(GetPosition(), drawingContext.GetCurrentTransform());

    auto primitiveBatch = drawingContext.GetPrimitiveBatch();

    const auto sliderWidth2 = static_cast<float>(GetWidth() * ((value - minimum) / (maximum - minimum)));
    const auto controlPosition2 = static_cast<float>((GetWidth() - GetHeight()) * ((value - minimum) / (maximum - minimum)));

    const auto sliderHeight = static_cast<float>(GetHeight());
    const auto paddingY = (static_cast<float>(GetHeight()) - sliderHeight) * 0.5f;

    primitiveBatch->DrawRectangle(
        Matrix3x2::Identity,
        Vector2{0.0f, paddingY} + MathHelper::ToVector2(globalPos),
        static_cast<float>(GetWidth()),
        sliderHeight,
        trackColor);
    primitiveBatch->DrawRectangle(
        Matrix3x2::Identity,
        Vector2{0.0f, paddingY} + MathHelper::ToVector2(globalPos),
        sliderWidth2,
        sliderHeight,
        fillColor);

    const auto thumbSize = static_cast<float>(GetHeight());

    if (isEnabled && isDragging && isThumbVisible) {
        // NOTE: Draw Thumb
        constexpr float pixel = 2.0f;

        auto pos = Vector2(controlPosition2 - pixel, -pixel);
        auto size = Vector2(thumbSize + 2 * pixel, thumbSize + 2 * pixel);

        primitiveBatch->DrawRectangle(
            Matrix3x2::Identity,
            pos + MathHelper::ToVector2(globalPos),
            size.X,
            size.Y,
            colorScheme->SliderThumbColorFocused);
    }

    if (isEnabled && isThumbVisible) {
        // NOTE: Draw Thumb
        auto pos = Vector2(controlPosition2, 0);
        primitiveBatch->DrawRectangle(
            Matrix3x2::Identity,
            pos + MathHelper::ToVector2(globalPos),
            thumbSize,
            thumbSize,
            colorScheme->SliderTrackColorBase);
    }
    primitiveBatch->Flush();

    if (isTextVisible) {
        auto spriteBatch = drawingContext.GetSpriteBatch();
        auto spriteFont = drawingContext.GetFont(FontWeight::Normal, FontSize::Medium);
        auto text = StringHelper::Format("%5.3lf", value);

        constexpr float textMarginX = 2.0f;
        constexpr float textMarginY = 3.5f;
        Vector2 textPosition;
        textPosition.Y = GetHeight() * 0.5f + textMarginY;
        Vector2 originPivot;
        if (value < MathHelper::Lerp(minimum, maximum, 0.6)) {
            originPivot = Vector2{1.0f, 0.5f};
            textPosition.X = GetWidth() - textMarginX;
        }
        else {
            originPivot = Vector2{0.0f, 0.5f};
            textPosition.X = textMarginX;
        }
        textPosition += MathHelper::ToVector2(globalPos);
        spriteFont->Draw(*spriteBatch, text, textPosition, colorScheme->SliderTextColor, 0.0f, originPivot, 1.0f);
    }
}

} // namespace Pomdog::GUI
