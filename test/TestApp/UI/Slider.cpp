//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "Slider.hpp"
#include "DrawingContext.hpp"
#include "PointerPoint.hpp"
#include "UIHelper.hpp"

namespace Pomdog {
namespace UI {
namespace {

namespace SliderColorScheme {

static Color BorderColor {0, 140, 198, 255};
static Color FillColor1 {27, 161, 226, 255};
static Color FillColor2 {56, 190, 255, 255};
static Color DisabledFillColor {150, 161, 167, 255};
static Color TrackColor {198, 198, 198, 255};
static Color ThumbColor = Color::Black;
static Color FocusedThumbColor {229, 20, 0, 255};

}

}// unnamed namespace
//-----------------------------------------------------------------------
Slider::Slider(double minimumIn, double maximumIn)
	: Control(Matrix3x2::Identity, 120, 12)
	, minimum(minimumIn)
	, maximum(maximumIn)
	, value(minimumIn)
	, borderColor(SliderColorScheme::BorderColor)
	, fillColor(SliderColorScheme::FillColor1)
	, trackColor(SliderColorScheme::TrackColor)
	, isDragging(false)
	, isEnabled(true)
{
	POMDOG_ASSERT(minimum < maximum);
	POMDOG_ASSERT(value >= minimum);
	POMDOG_ASSERT(value <= maximum);
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Properties
#endif
//-----------------------------------------------------------------------
void Slider::Value(double valueIn)
{ this->value = valueIn; }
//-----------------------------------------------------------------------
double Slider::Value() const
{ return value; }
//-----------------------------------------------------------------------
double Slider::Minimum() const
{ return minimum; }
//-----------------------------------------------------------------------
double Slider::Maximum() const
{ return maximum; }
//-----------------------------------------------------------------------
bool Slider::IsEnabled() const
{ return isEnabled; }
//-----------------------------------------------------------------------
void Slider::IsEnabled(bool isEnabledIn)
{
	this->isEnabled = isEnabledIn;
	if (isEnabled) {
		fillColor = SliderColorScheme::FillColor1;
	}
	else {
		fillColor = SliderColorScheme::DisabledFillColor;
	}
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Events
#endif
//-----------------------------------------------------------------------
void Slider::OnPointerCanceled(PointerPoint const& pointerPoint)
{
}
//-----------------------------------------------------------------------
void Slider::OnPointerCaptureLost(PointerPoint const& pointerPoint)
{
}
//-----------------------------------------------------------------------
void Slider::OnPointerWheelChanged(PointerPoint const& pointerPoint)
{
}
//-----------------------------------------------------------------------
void Slider::OnPointerEntered(PointerPoint const& pointerPoint)
{
	if (!isEnabled) {
		return;
	}

	ColorAnimation animation;
	animation.duration = 0.19f;
	animation.startColor = fillColor;
	animation.targetColor = SliderColorScheme::FillColor2;

	colorAnimation = animation;
}
//-----------------------------------------------------------------------
void Slider::OnPointerExited(PointerPoint const& pointerPoint)
{
	ColorAnimation animation;
	animation.duration = 0.15f;
	animation.startColor = fillColor;
	animation.targetColor = (isEnabled) ? SliderColorScheme::FillColor1:
		SliderColorScheme::DisabledFillColor;

	colorAnimation = animation;
}
//-----------------------------------------------------------------------
void Slider::OnPointerPressed(PointerPoint const& pointerPoint)
{
	if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != UI::PointerMouseEvent::LeftButtonPressed) {
		return;
	}
	
	if (!isEnabled) {
		return;
	}

	POMDOG_ASSERT(Width() > 0);
	
	// NOTE: float thumbOffset = thumbWidth / 2
	constexpr float thumbOffset = 5;
	
	auto pointInView = UIHelper::ConvertToChildSpace(pointerPoint.Position, GlobalTransform());
	auto amount = (pointInView.X - thumbOffset / 2) / (Width() - 2 * thumbOffset);
	value = MathHelper::Clamp(amount * (maximum - minimum) + minimum, minimum, maximum);
	isDragging = true;
}
//-----------------------------------------------------------------------
void Slider::OnPointerMoved(PointerPoint const& pointerPoint)
{
	if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != UI::PointerMouseEvent::LeftButtonPressed) {
		return;
	}
	
	if (!isEnabled) {
		return;
	}

	POMDOG_ASSERT(Width() > 0);

	// NOTE: float thumbOffset = thumbWidth / 2
	constexpr float thumbOffset = 5;
	
	auto pointInView = UIHelper::ConvertToChildSpace(pointerPoint.Position, GlobalTransform());
	auto amount = (pointInView.X - thumbOffset / 2) / (Width() - 2 * thumbOffset);
	value = MathHelper::Clamp(amount * (maximum - minimum) + minimum, minimum, maximum);
}
//-----------------------------------------------------------------------
void Slider::OnPointerReleased(PointerPoint const& pointerPoint)
{
	isDragging = false;
}
//-----------------------------------------------------------------------
void Slider::UpdateAnimation(DurationSeconds const& frameDuration)
{
	if (!colorAnimation) {
		return;
	}
	
	colorAnimation->time += frameDuration.count();
	colorAnimation->time = std::min(colorAnimation->time, colorAnimation->duration);
	
	fillColor = Color::SmoothStep(colorAnimation->startColor, colorAnimation->targetColor,
		colorAnimation->time / colorAnimation->duration);
	
	if (colorAnimation->time >= colorAnimation->duration) {
		colorAnimation = OptionalType::NullOptional;
	}
}
//-----------------------------------------------------------------------
void Slider::OnRenderSizeChanged(std::uint32_t widthIn, std::uint32_t heightIn)
{
	Width(widthIn);
	Height(heightIn);
}
//-----------------------------------------------------------------------
void Slider::Draw(DrawingContext & drawingContext)
{
	//MathHelper::Clamp(value, minimum, maximum);

	POMDOG_ASSERT(value >= minimum);
	POMDOG_ASSERT(value <= maximum);
	
	auto sliderWidth2 = Width() * ((value - minimum) / (maximum - minimum));
	auto controlPosition2 = (Width() - Height()) * ((value - minimum) / (maximum - minimum));
	
	auto transform = Transform() * drawingContext.Top();
	
	drawingContext.DrawRectangle(transform, trackColor, Rectangle(0, 0, Width(), Height()));
	drawingContext.DrawRectangle(transform, fillColor, Rectangle(0, 0, sliderWidth2, Height()));
	
	if (isEnabled && isDragging)
	{
		constexpr float pixel = 2.0f;
		
		auto pos = Vector2(controlPosition2 - pixel, -pixel);
		auto size = Vector2(Height() + 2 * pixel, Height() + 2 * pixel);
		
		drawingContext.DrawRectangle(transform, SliderColorScheme::FocusedThumbColor,
			Rectangle(pos.X, pos.Y, size.X, size.Y));
	}
	
	if (isEnabled)
	{
		auto pos = Vector2(controlPosition2, 0);
		drawingContext.DrawRectangle(transform, SliderColorScheme::ThumbColor,
			Rectangle(pos.X, pos.Y, Height(), Height()));
	}
}
//-----------------------------------------------------------------------
}// namespace UI
}// namespace Pomdog
