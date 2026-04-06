// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/slider.h"
#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/utility/string_format.h"

namespace pomdog::gui {

Slider::Slider(
    const std::shared_ptr<UIEventDispatcher>& dispatcher,
    double minimumIn,
    double maximumIn)
    : Widget(dispatcher)
    , minimum_(minimumIn)
    , maximum_(maximumIn)
    , value_(minimumIn)
    , isDragging_(false)
    , isEnabled_(true)
    , isTextVisible_(true)
    , isThumbVisible_(false)
    , isPointerEntered_(false)
{
    POMDOG_ASSERT(minimum_ < maximum_);
    POMDOG_ASSERT(value_ >= minimum_);
    POMDOG_ASSERT(value_ <= maximum_);

    setSize(120, 18);
    setCursor(MouseCursor::ResizeHorizontal);
}

void Slider::setValue(double valueIn)
{
    if (value_ == valueIn) {
        return;
    }
    value_ = valueIn;
    ValueChanged(value_);
}

double Slider::getValue() const noexcept
{
    return value_;
}

double Slider::getMinimum() const noexcept
{
    return minimum_;
}

void Slider::setMinimum(double minimumIn)
{
    if (minimum_ == minimumIn) {
        return;
    }
    minimum_ = minimumIn;
    if (minimum_ > value_) {
        value_ = minimum_;
        ValueChanged(value_);
    }
}

void Slider::setMaximum(double maximumIn)
{
    if (maximum_ == maximumIn) {
        return;
    }
    maximum_ = maximumIn;
    if (maximum_ < value_) {
        value_ = maximum_;
        ValueChanged(value_);
    }
}

double Slider::getMaximum() const noexcept
{
    return maximum_;
}

bool Slider::isEnabled() const noexcept
{
    return isEnabled_;
}

void Slider::setEnabled(bool enabledIn) noexcept
{
    isEnabled_ = enabledIn;
}

void Slider::setTextVisible(bool isTextVisibleIn)
{
    isTextVisible_ = isTextVisibleIn;
}

void Slider::setThumbVisible(bool isThumbVisibleIn)
{
    isThumbVisible_ = isThumbVisibleIn;
}

VerticalAlignment Slider::getVerticalAlignment() const noexcept
{
    return VerticalAlignment::Top;
}

bool Slider::getSizeToFitContent() const noexcept
{
    return false;
}

void Slider::onEnter()
{
}

void Slider::onPointerEntered([[maybe_unused]] const PointerPoint& pointerPoint)
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

void Slider::onPointerExited([[maybe_unused]] const PointerPoint& pointerPoint)
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

void Slider::onPointerPressed([[maybe_unused]] const PointerPoint& pointerPoint)
{
    if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != PointerMouseEvent::LeftButtonPressed) {
        return;
    }

    if (!isEnabled_) {
        return;
    }

    POMDOG_ASSERT(getWidth() > 0);

    // NOTE: double thumbOffset = thumbWidth / 2
    constexpr double thumbOffset = 5.0;

    auto pointInView = UIHelper::projectToChildSpace(pointerPoint.Position, getGlobalPosition());
    auto amount = (pointInView.x - thumbOffset / 2) / (getWidth() - 2 * thumbOffset);
    setValue(math::clamp(amount * (maximum_ - minimum_) + minimum_, minimum_, maximum_));
    isDragging_ = true;
}

void Slider::onPointerMoved(const PointerPoint& pointerPoint)
{
    if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != PointerMouseEvent::LeftButtonPressed) {
        return;
    }

    if (!isEnabled_) {
        return;
    }

    POMDOG_ASSERT(getWidth() > 0);

    // NOTE: double thumbOffset = thumbWidth / 2
    constexpr double thumbOffset = 5.0;

    auto pointInView = UIHelper::projectToChildSpace(pointerPoint.Position, getGlobalPosition());
    auto amount = (pointInView.x - thumbOffset / 2) / (getWidth() - 2 * thumbOffset);
    setValue(math::clamp(amount * (maximum_ - minimum_) + minimum_, minimum_, maximum_));
}

void Slider::onPointerReleased([[maybe_unused]] const PointerPoint& pointerPoint)
{
    isDragging_ = false;
}

void Slider::updateAnimation(const Duration& frameDuration)
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

void Slider::draw(DrawingContext& drawingContext)
{
    POMDOG_ASSERT(value_ >= minimum_);
    POMDOG_ASSERT(value_ <= maximum_);

    const auto* colorScheme = drawingContext.getColorScheme();
    POMDOG_ASSERT(colorScheme != nullptr);

    auto fillColor = colorScheme->SliderFillColorBase;
    auto trackColor = colorScheme->SliderTrackColorBase;

    if (!isEnabled_) {
        fillColor = colorScheme->SliderFillColorDisabled;
    }
    else if (colorAnimation_ != std::nullopt) {
        POMDOG_ASSERT(isEnabled_);
        auto startColor = colorScheme->SliderFillColorBase;
        auto targetColor = colorScheme->SliderFillColorHovered;
        if (!isPointerEntered_) {
            std::swap(startColor, targetColor);
        }
        const auto scaleFactor = colorAnimation_->time / colorAnimation_->duration;
        fillColor = math::smoothstep(startColor, targetColor, scaleFactor);
    }
    else if (isPointerEntered_) {
        POMDOG_ASSERT(isEnabled_);
        POMDOG_ASSERT(colorAnimation_ == std::nullopt);
        fillColor = colorScheme->SliderFillColorHovered;
    }
    else {
        POMDOG_ASSERT(isEnabled_);
        POMDOG_ASSERT(colorAnimation_ == std::nullopt);
        POMDOG_ASSERT(!isPointerEntered_);
        fillColor = colorScheme->SliderFillColorBase;
    }

    if (isPointerEntered_) {
        trackColor = colorScheme->SliderTrackColorHovered;
    }

    auto globalPos = UIHelper::projectToWorldSpace(getPosition(), drawingContext.getCurrentTransform());

    auto primitiveBatch = drawingContext.getPrimitiveBatch();

    const auto sliderWidth2 = static_cast<float>(getWidth() * ((value_ - minimum_) / (maximum_ - minimum_)));
    const auto controlPosition2 = static_cast<float>((getWidth() - getHeight()) * ((value_ - minimum_) / (maximum_ - minimum_)));

    const auto sliderHeight = static_cast<float>(getHeight());
    const auto paddingY = (static_cast<float>(getHeight()) - sliderHeight) * 0.5f;

    primitiveBatch->drawRectangle(
        Matrix3x2::createIdentity(),
        Vector2{0.0f, paddingY} + math::toVector2(globalPos),
        static_cast<float>(getWidth()),
        sliderHeight,
        trackColor);
    primitiveBatch->drawRectangle(
        Matrix3x2::createIdentity(),
        Vector2{0.0f, paddingY} + math::toVector2(globalPos),
        sliderWidth2,
        sliderHeight,
        fillColor);

    const auto thumbSize = static_cast<float>(getHeight());

    if (isEnabled_ && isDragging_ && isThumbVisible_) {
        // NOTE: draw Thumb
        constexpr float pixel = 2.0f;

        auto pos = Vector2(controlPosition2 - pixel, -pixel);
        auto size = Vector2(thumbSize + 2 * pixel, thumbSize + 2 * pixel);

        primitiveBatch->drawRectangle(
            Matrix3x2::createIdentity(),
            pos + math::toVector2(globalPos),
            size.x,
            size.y,
            colorScheme->SliderThumbColorFocused);
    }

    if (isEnabled_ && isThumbVisible_) {
        // NOTE: draw Thumb
        auto pos = Vector2(controlPosition2, 0);
        primitiveBatch->drawRectangle(
            Matrix3x2::createIdentity(),
            pos + math::toVector2(globalPos),
            thumbSize,
            thumbSize,
            colorScheme->SliderTrackColorBase);
    }
    drawingContext.flushPrimitiveBatch();

    if (isTextVisible_) {
        auto spriteBatch = drawingContext.getSpriteBatch();
        auto spriteFont = drawingContext.getFont(FontWeight::Normal, FontSize::Medium);
        auto text = pomdog::format("{:5.3f}", value_);

        constexpr float textMarginX = 2.0f;
        constexpr float textMarginY = 3.5f;
        Vector2 textPosition;
        textPosition.y = getHeight() * 0.5f + textMarginY;
        Vector2 originPivot;
        if (value_ < math::lerp(minimum_, maximum_, 0.6)) {
            originPivot = Vector2{1.0f, 0.5f};
            textPosition.x = getWidth() - textMarginX;
        }
        else {
            originPivot = Vector2{0.0f, 0.5f};
            textPosition.x = textMarginX;
        }
        textPosition += math::toVector2(globalPos);
        spriteFont->draw(*spriteBatch, text, textPosition, colorScheme->SliderTextColor, 0.0f, originPivot, 1.0f);
    }
}

} // namespace pomdog::gui
