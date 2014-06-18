//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "ToggleSwitch.hpp"
#include "DrawingContext.hpp"
#include "PointerPoint.hpp"
#include "UIHelper.hpp"

namespace Pomdog {
namespace UI {
//-----------------------------------------------------------------------
ToggleSwitch::ToggleSwitch()
	: Control(Matrix3x2::Identity, 54, 20)
	, isEnabled(true)
	, isOn(true)
{
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Properties
#endif
//-----------------------------------------------------------------------
bool ToggleSwitch::IsOn() const
{ return isOn; }
//-----------------------------------------------------------------------
void ToggleSwitch::IsOn(bool isOnIn)
{ this->isOn = isOnIn; }
//-----------------------------------------------------------------------
bool ToggleSwitch::IsEnabled() const
{ return isEnabled; }
//-----------------------------------------------------------------------
void ToggleSwitch::IsEnabled(bool isEnabledIn)
{ this->isEnabled = isEnabledIn; }
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - Events
#endif
//-----------------------------------------------------------------------
void ToggleSwitch::OnPointerPressed(PointerPoint const& pointerPoint)
{
}
//-----------------------------------------------------------------------
void ToggleSwitch::OnPointerReleased(PointerPoint const& pointerPoint)
{
	if (!isEnabled) {
		return;
	}
	isOn = !isOn;
}
//-----------------------------------------------------------------------
void ToggleSwitch::Draw(DrawingContext & drawingContext)
{
	auto transform = Transform() * drawingContext.Top();
	
	Color backgroundColor = {121, 121, 121, 255};
	Color innerColor = {109, 109, 109, 255};
	Color thumbColor = Color::White;
	
	const Color toggleOnInnerColor = {4, 125, 137, 255};
	const Color disabledInnerColor = {81, 81, 81, 255};
	const Color disabledThumbColor = {137, 137, 137, 255};

	constexpr int lineThickness = 2;

	if (!isEnabled)
	{
		innerColor = disabledInnerColor;
		thumbColor = disabledThumbColor;
	}
	
	if (isOn && isEnabled)
	{
		innerColor = toggleOnInnerColor;
	}

	drawingContext.DrawRectangle(transform, backgroundColor, Rectangle(0, 0, Width(), lineThickness));
	drawingContext.DrawRectangle(transform, backgroundColor, Rectangle(0, Height() - lineThickness, Width(), lineThickness));
	
	constexpr int innerLineThickness = 1;
	drawingContext.DrawRectangle(transform, innerColor, Rectangle(lineThickness + innerLineThickness, lineThickness + innerLineThickness,
		Width() - (lineThickness * 2 + innerLineThickness * 2),
		Height() - (lineThickness * 2 + innerLineThickness * 2)));
	
	constexpr std::uint16_t thumbWidth = 12;
	if (isOn) {
		drawingContext.DrawRectangle(transform, backgroundColor, Rectangle(0, 0, lineThickness, Height()));
		drawingContext.DrawRectangle(transform, thumbColor, Rectangle(Width() - thumbWidth, 0, thumbWidth, Height()));
	}
	else {
		drawingContext.DrawRectangle(transform, backgroundColor, Rectangle(Width() - lineThickness, 0, lineThickness, Height()));
		drawingContext.DrawRectangle(transform, thumbColor, Rectangle(0, 0, thumbWidth, Height()));
	}
	
	if (isOn) {
		drawingContext.DrawString(transform * Matrix3x2::CreateTranslation(Vector2(Width() + 5, 2)),
			{181,181,181,255}, FontWeight::Bold, FontSize::Medium, u8"On");
	}
	else {
		drawingContext.DrawString(transform * Matrix3x2::CreateTranslation(Vector2(Width() + 5, 2)),
			{181,181,181,255}, FontWeight::Bold, FontSize::Medium, u8"Off");
	}
}
//-----------------------------------------------------------------------
}// namespace UI
}// namespace Pomdog
