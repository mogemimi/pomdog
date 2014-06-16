//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "StackPanel.hpp"
#include "DrawingContext.hpp"
#include "PointerPoint.hpp"
#include "UIHelper.hpp"

namespace Pomdog {
namespace UI {
//-----------------------------------------------------------------------
StackPanel::StackPanel(std::uint32_t widthIn, std::uint32_t heightIn)
	: Panel(Matrix3x2::Identity, widthIn, heightIn)
{
}
//-----------------------------------------------------------------------
void StackPanel::OnPointerCanceled(PointerPoint const& pointerPoint)
{
}
//-----------------------------------------------------------------------
void StackPanel::OnPointerCaptureLost(PointerPoint const& pointerPoint)
{
}
//-----------------------------------------------------------------------
void StackPanel::OnPointerWheelChanged(PointerPoint const& pointerPoint)
{
}
//-----------------------------------------------------------------------
void StackPanel::OnPointerEntered(PointerPoint const& pointerPoint)
{
}
//-----------------------------------------------------------------------
void StackPanel::OnPointerExited(PointerPoint const& pointerPoint)
{
}
//-----------------------------------------------------------------------
void StackPanel::OnPointerPressed(PointerPoint const& pointerPoint)
{
	Rectangle captionBar{0, 0, Width(), 32};
	
	auto pointInView = UIHelper::ConvertToChildSpace(pointerPoint.Position, GlobalTransform());
	if (!captionBar.Intersects(pointInView)) {
		return;
	}

	startTouchPoint = Vector2(pointInView.X, pointInView.Y);
}
//-----------------------------------------------------------------------
void StackPanel::OnPointerMoved(PointerPoint const& pointerPoint)
{
	if (!startTouchPoint) {
		return;
	}
	
	auto pointInView = UIHelper::ConvertToChildSpace(pointerPoint.Position, GlobalTransform());
	auto position = Vector2(pointInView.X, pointInView.Y);

	auto tangent = position - *startTouchPoint;
	auto distanceSquared = tangent.LengthSquared();

	if (distanceSquared >= 1.4143f)
	{
		Transform(Transform() * Matrix3x2::CreateTranslation(tangent));
		
		///@note recalculate position in current coordinate system
		pointInView = UIHelper::ConvertToChildSpace(pointerPoint.Position, GlobalTransform());
		position = Vector2(pointInView.X, pointInView.Y);
		startTouchPoint = position;
		
		for (auto & child: Children)
		{
			POMDOG_ASSERT(child);
			child->MarkParentTransformDirty();
		}
	}
}
//-----------------------------------------------------------------------
void StackPanel::OnPointerReleased(PointerPoint const& pointerPoint)
{
	if (!startTouchPoint) {
		return;
	}
	
	startTouchPoint = OptionalType::NullOptional;
}
//-----------------------------------------------------------------------
void StackPanel::OnRenderSizeChanged(std::uint32_t widthIn, std::uint32_t heightIn)
{
	Width(widthIn);
	Height(heightIn);
}
//-----------------------------------------------------------------------
void StackPanel::Draw(DrawingContext & drawingContext)
{
	auto transform = Transform() * drawingContext.Top();

	drawingContext.DrawRectangle(transform, Color{51, 51, 51, 224},
		Rectangle(0, 0, Width(), Height()));
	
	drawingContext.Push(transform);
	for (auto & child: Children)
	{
		POMDOG_ASSERT(child);
		child->Draw(drawingContext);
	}
	drawingContext.Pop();
}
//-----------------------------------------------------------------------
void StackPanel::UpdateAnimation(DurationSeconds const& frameDuration)
{
}
//-----------------------------------------------------------------------
}// namespace UI
}// namespace Pomdog
