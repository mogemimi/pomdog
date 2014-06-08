//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "UIElementHierarchy.hpp"
#include "DrawingContext.hpp"

namespace Pomdog {
namespace UI {
//-----------------------------------------------------------------------
std::shared_ptr<UIElement> UIElementHierarchy::Find(Point2D const& position)
{
	for (auto & node: nodes)
	{
		if (node->Intersects(position)) {
			return node;
		}
	}
	return {};
}
//-----------------------------------------------------------------------
void UIElementHierarchy::Touch(MouseState const& mouseState)
{
	auto position = mouseState.Position;
	
	if (!pointerState)
	{
		if (auto node = Find(position))
		{
			POMDOG_ASSERT(!pointerState);
			PointerEntered(position, mouseState, node);
		}
	}
	
	if (!pointerState) {
		return;
	}
	
	switch (pointerState->pointerPoint.Event) {
	case PointerEventType::Canceled:
		break;
	case PointerEventType::CaptureLost:
		break;
	case PointerEventType::Released:
	case PointerEventType::Entered: {
		auto node = Find(position);
		if (!node || node != pointerState->focusedElement) {
			PointerExited(position);
			POMDOG_ASSERT(!pointerState);
		}
		else if (auto pointerMouseEvent = FindPointerMouseEvent(mouseState)) {
			pointerState->pointerPoint.MouseEvent = pointerMouseEvent;
			POMDOG_ASSERT(CheckMouseButton(mouseState, *pointerMouseEvent) == ButtonState::Pressed);
		
			POMDOG_ASSERT(node == pointerState->focusedElement);
			POMDOG_ASSERT(pointerState->focusedElement->Intersects(position));
			PointerPressed(position);
		}
		break;
	}
	case PointerEventType::Exited:
		break;
	case PointerEventType::Pressed:
	case PointerEventType::Moved:
		if (pointerState->pointerPoint.MouseEvent
			&& CheckMouseButton(mouseState, *pointerState->pointerPoint.MouseEvent) == ButtonState::Pressed) {
			PointerMoved(position);
		}
		else {
			PointerReleased(position);
		}
		break;
	case PointerEventType::WheelChanged:
		break;
	}
	
	if (pointerState)
	{
		auto oldMouseWheelDelta = pointerState->pointerPoint.MouseWheelDelta;
		pointerState->pointerPoint.MouseWheelDelta = mouseState.ScrollWheel - pointerState->PrevScrollWheel;
		pointerState->PrevScrollWheel = mouseState.ScrollWheel;
		
		if (oldMouseWheelDelta != pointerState->pointerPoint.MouseWheelDelta)
		{
			POMDOG_ASSERT(pointerState->focusedElement);
			pointerState->focusedElement->OnPointerWheelChanged(pointerState->pointerPoint);
		}
	}
}
//-----------------------------------------------------------------------
void UIElementHierarchy::PointerEntered(Point2D const& position, MouseState const& mouseState, std::shared_ptr<UI::UIElement> const& node)
{
	POMDOG_ASSERT(!pointerState);
	pointerState = MakeUnique<PointerState>();

	pointerState->pointerPoint.Event = PointerEventType::Entered;
	pointerState->pointerPoint.Position = position;
	pointerState->pointerPoint.Id = 0;
	pointerState->pointerPoint.MouseWheelDelta = 0;
	pointerState->PrevScrollWheel = mouseState.ScrollWheel;

	node->OnPointerEntered(pointerState->pointerPoint);
	pointerState->focusedElement = node;
}
//-----------------------------------------------------------------------
void UIElementHierarchy::PointerExited(Point2D const& position)
{
	POMDOG_ASSERT(pointerState);
	POMDOG_ASSERT(pointerState->focusedElement);

	pointerState->pointerPoint.Event = PointerEventType::Exited;
	pointerState->pointerPoint.Position = position;

	pointerState->focusedElement->OnPointerExited(pointerState->pointerPoint);
	pointerState.reset();
}
//-----------------------------------------------------------------------
void UIElementHierarchy::PointerPressed(Point2D const& position)
{
	POMDOG_ASSERT(pointerState);

	pointerState->pointerPoint.Event = PointerEventType::Pressed;
	pointerState->pointerPoint.Position = position;
	pointerState->pointerPoint.Id = 0;

	pointerState->focusedElement->OnPointerPressed(pointerState->pointerPoint);
}

void UIElementHierarchy::PointerMoved(Point2D const& position)
{
	POMDOG_ASSERT(pointerState);
	POMDOG_ASSERT(pointerState->focusedElement);
	
	pointerState->pointerPoint.Event = PointerEventType::Moved;
	pointerState->pointerPoint.Position = position;

	pointerState->focusedElement->OnPointerMoved(pointerState->pointerPoint);
}
//-----------------------------------------------------------------------
void UIElementHierarchy::PointerReleased(Point2D const& position)
{
	POMDOG_ASSERT(pointerState);
	POMDOG_ASSERT(pointerState->focusedElement);

	pointerState->pointerPoint.Event = PointerEventType::Released;
	pointerState->pointerPoint.Position = position;

	pointerState->focusedElement->OnPointerReleased(pointerState->pointerPoint);
}
//-----------------------------------------------------------------------
void UIElementHierarchy::AddChild(std::shared_ptr<UI::UIElement> const& node)
{
	nodes.push_back(node);
	Sort();
}
//-----------------------------------------------------------------------
void UIElementHierarchy::AddChild(std::shared_ptr<UI::UIElement> && node)
{
	nodes.push_back(std::move(node));
	Sort();
}
//-----------------------------------------------------------------------
void UIElementHierarchy::UpdateAnimation(DurationSeconds const& frameDuration)
{
	for (auto & node: nodes)
	{
		POMDOG_ASSERT(node);
		node->UpdateAnimation(frameDuration);
	}
}
//-----------------------------------------------------------------------
void UIElementHierarchy::Draw(DrawingContext & drawingContext)
{
	std::for_each(nodes.rbegin(), nodes.rend(), [&drawingContext](std::shared_ptr<UI::UIElement> const& node)
	{
		node->Draw(drawingContext);
	});
}
//-----------------------------------------------------------------------
Optional<UI::PointerMouseEvent> UIElementHierarchy::FindPointerMouseEvent(MouseState const& mouseState) const
{
	using Pomdog::UI::PointerMouseEvent;
	if (mouseState.LeftButton == ButtonState::Pressed) {
		return PointerMouseEvent::LeftButtonPressed;
	}
	else if (mouseState.RightButton == ButtonState::Pressed) {
		return PointerMouseEvent::RightButtonPressed;
	}
	else if (mouseState.MiddleButton == ButtonState::Pressed) {
		return PointerMouseEvent::MiddleButtonPressed;
	}
	else if (mouseState.XButton1 == ButtonState::Pressed) {
		return PointerMouseEvent::XButton1Pressed;
	}
	else if (mouseState.XButton2 == ButtonState::Pressed) {
		return PointerMouseEvent::XButton2Pressed;
	}
	return OptionalType::NullOptional;
}
//-----------------------------------------------------------------------
ButtonState UIElementHierarchy::CheckMouseButton(MouseState const& mouseState, UI::PointerMouseEvent const& pointerMouseEvent) const
{
	using Pomdog::UI::PointerMouseEvent;
	switch (pointerMouseEvent) {
	case PointerMouseEvent::LeftButtonPressed:
		return mouseState.LeftButton;
	case PointerMouseEvent::MiddleButtonPressed:
		return mouseState.MiddleButton;
	case PointerMouseEvent::RightButtonPressed:
		return mouseState.RightButton;
	case PointerMouseEvent::XButton1Pressed:
		return mouseState.XButton1;
	case PointerMouseEvent::XButton2Pressed:
		return mouseState.XButton2;
		break;
	case PointerMouseEvent::ScrollWheel:
		break;
	}
	return ButtonState::Released;
}
//-----------------------------------------------------------------------
void UIElementHierarchy::Sort()
{
	std::sort(std::begin(nodes), std::end(nodes),
		[](std::shared_ptr<UI::UIElement> const& a, std::shared_ptr<UI::UIElement> const& b){
			return a->drawOrder < b->drawOrder; });
}
//-----------------------------------------------------------------------
}// namespace UI
}// namespace Pomdog
