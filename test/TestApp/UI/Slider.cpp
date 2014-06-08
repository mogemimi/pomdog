//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "Slider.hpp"
#include "DrawingContext.hpp"

namespace Pomdog {
namespace UI {
namespace {

namespace SliderColorScheme {

static Color BorderColor {0, 140, 198, 255};
static Color FillColor1 {0, 140, 198, 255};
static Color FillColor2 {0, 157, 210, 255};
static Color TrackColor {198, 198, 198, 255};
static Color ThumbColor = Color::Black;
static Color FocusedThumbColor {229, 20, 0, 255};

}

}// unnamed namespace
//-----------------------------------------------------------------------
Slider::Slider(double minimumIn, double maximumIn)
	: height(12)
	, width(120)
	, minimum(minimumIn)
	, maximum(maximumIn)
	, value(minimumIn)
	, borderColor(SliderColorScheme::BorderColor)
	, fillColor(SliderColorScheme::FillColor1)
	, trackColor(SliderColorScheme::TrackColor)
	, isDragging(false)
{
	POMDOG_ASSERT(minimum < maximum);
	POMDOG_ASSERT(value >= minimum);
	POMDOG_ASSERT(value <= maximum);
	
	RenderTransform.Position = {50, 100};
}
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
	fillColor = SliderColorScheme::FillColor2;
}
//-----------------------------------------------------------------------
void Slider::OnPointerExited(PointerPoint const& pointerPoint)
{
	fillColor = SliderColorScheme::FillColor1;
}
//-----------------------------------------------------------------------
void Slider::OnPointerPressed(PointerPoint const& pointerPoint)
{
	if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != UI::PointerMouseEvent::LeftButtonPressed) {
		return;
	}

	POMDOG_ASSERT(width > 0);
	auto amount = (pointerPoint.Position.X - RenderTransform.Position.X) / width;
	value = MathHelper::Clamp(amount * (maximum - minimum) + minimum, minimum, maximum);
	isDragging = true;
}
//-----------------------------------------------------------------------
void Slider::OnPointerMoved(PointerPoint const& pointerPoint)
{
	if (pointerPoint.MouseEvent && *pointerPoint.MouseEvent != UI::PointerMouseEvent::LeftButtonPressed) {
		return;
	}

	POMDOG_ASSERT(width > 0);
	auto amount = (pointerPoint.Position.X - RenderTransform.Position.X) / width;
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
}
//-----------------------------------------------------------------------
void Slider::Draw(DrawingContext & drawingContext)
{
	///@todo badcode
	bounds.X = RenderTransform.Position.X;
	bounds.Y = RenderTransform.Position.Y;
	bounds.Width = width;
	bounds.Height = height;
	
	auto sliderWidth2 = width * ((value - minimum) / (maximum - minimum));
	auto controlPosition2 = (width - height) * ((value - minimum) / (maximum - minimum));
	
	std::int32_t x = RenderTransform.Position.X;
	std::int32_t y = RenderTransform.Position.Y;
	
	drawingContext.DrawRectangle(trackColor, Rectangle(x, y, width, height));
	drawingContext.DrawRectangle(fillColor, Rectangle(x, y, sliderWidth2, height));
	
	if (isDragging)
	{
		constexpr float pixel = 2.0f;
		
		auto pos = RenderTransform.Position + Vector2(controlPosition2 - pixel, -pixel);
		auto size = Vector2(height + 2 * pixel, height + 2 * pixel);
		
		drawingContext.DrawRectangle(SliderColorScheme::FocusedThumbColor,
			Rectangle(pos.X, pos.Y, size.X, size.Y));
	}
	
	{
		auto pos = RenderTransform.Position + Vector2(controlPosition2, 0);
		drawingContext.DrawRectangle(SliderColorScheme::ThumbColor,
			Rectangle(pos.X, pos.Y, height, height));
	}
}
//-----------------------------------------------------------------------
}// namespace UI
}// namespace Pomdog
