//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "UIEventDispatcher.hpp"
#include "UIHelper.hpp"
#include "detail/UIEventConnection.hpp"
#include <algorithm>

namespace Pomdog {
namespace UI {
namespace {

bool Intersects(Point2D const& position, UIElement & element)
{
	Rectangle bounds(0, 0, element.Width(), element.Height());
	auto positionInChild = UIHelper::ConvertToChildSpace(position, element.GlobalTransform());
	return bounds.Intersects(positionInChild);
}

}// unnamed namespace
//-----------------------------------------------------------------------
UIEventDispatcher::UIEventDispatcher(std::shared_ptr<GameWindow> const& windowIn)
	: window(windowIn)
{}
//-----------------------------------------------------------------------
void UIEventDispatcher::UpdateChildren()
{
	if (!subscribeRequests.Added.empty())
	{
		children.insert(std::end(children), std::begin(subscribeRequests.Added), std::end(subscribeRequests.Added));
		subscribeRequests.Added.clear();
	}

	if (!subscribeRequests.Removed.empty())
	{
		for (auto & child: subscribeRequests.Removed)
		{
			children.erase(std::remove_if(std::begin(children), std::end(children),
				[&child](std::shared_ptr<UIElement> const& p){ return p == child.lock(); }), std::end(children));
		}
	}

	Sort();
	POMDOG_ASSERT(std::end(children) == std::unique(std::begin(children), std::end(children)));
}
//-----------------------------------------------------------------------
std::shared_ptr<UIView> UIEventDispatcher::Find(Point2D const& position)
{
	for (auto & child: children)
	{
		if (Intersects(position, *child)) {
			return child;
		}
	}
	return {};
}
//-----------------------------------------------------------------------
void UIEventDispatcher::Touch(MouseState const& mouseState)
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
			POMDOG_ASSERT(Intersects(position, *pointerState->focusedElement));
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
void UIEventDispatcher::PointerEntered(Point2D const& position, MouseState const& mouseState, std::shared_ptr<UIView> const& node)
{
	POMDOG_ASSERT(!pointerState);
	pointerState = std::make_unique<PointerState>();

	pointerState->pointerPoint.Event = PointerEventType::Entered;
	pointerState->pointerPoint.Position = position;
	pointerState->pointerPoint.Id = 0;
	pointerState->pointerPoint.MouseWheelDelta = 0;
	pointerState->PrevScrollWheel = mouseState.ScrollWheel;

	node->OnPointerEntered(pointerState->pointerPoint);
	if (node->CurrentCursor()) {
		window->SetMouseCursor(*node->CurrentCursor());
	}
	else {
		window->SetMouseCursor(MouseCursor::Arrow);
	}
	
	pointerState->focusedElement = node;
}
//-----------------------------------------------------------------------
void UIEventDispatcher::PointerExited(Point2D const& position)
{
	POMDOG_ASSERT(pointerState);
	POMDOG_ASSERT(pointerState->focusedElement);

	pointerState->pointerPoint.Event = PointerEventType::Exited;
	pointerState->pointerPoint.Position = position;

	pointerState->focusedElement->OnPointerExited(pointerState->pointerPoint);
	pointerState.reset();

	window->SetMouseCursor(MouseCursor::Arrow);
}
//-----------------------------------------------------------------------
void UIEventDispatcher::PointerPressed(Point2D const& position)
{
	POMDOG_ASSERT(pointerState);

	pointerState->pointerPoint.Event = PointerEventType::Pressed;
	pointerState->pointerPoint.Position = position;
	pointerState->pointerPoint.Id = 0;

	pointerState->focusedElement->OnPointerPressed(pointerState->pointerPoint);
}
//-----------------------------------------------------------------------
void UIEventDispatcher::PointerMoved(Point2D const& position)
{
	POMDOG_ASSERT(pointerState);
	POMDOG_ASSERT(pointerState->focusedElement);
	
	pointerState->pointerPoint.Event = PointerEventType::Moved;
	pointerState->pointerPoint.Position = position;

	pointerState->focusedElement->OnPointerMoved(pointerState->pointerPoint);
}
//-----------------------------------------------------------------------
void UIEventDispatcher::PointerReleased(Point2D const& position)
{
	POMDOG_ASSERT(pointerState);
	POMDOG_ASSERT(pointerState->focusedElement);

	pointerState->pointerPoint.Event = PointerEventType::Released;
	pointerState->pointerPoint.Position = position;

	pointerState->focusedElement->OnPointerReleased(pointerState->pointerPoint);
}
//-----------------------------------------------------------------------
Details::UIEventConnection UIEventDispatcher::Connect(std::shared_ptr<UIView> const& child)
{
	std::shared_ptr<SubscribeRequestDispatcherType> sharedDispatcher(
		shared_from_this(), &subscribeRequests);
	Details::UIEventConnection connection {std::move(sharedDispatcher), child};

	subscribeRequests.AddChild(child);
	return std::move(connection);
}
//-----------------------------------------------------------------------
Details::UIEventConnection UIEventDispatcher::Connect(std::shared_ptr<UIView> && child)
{
	std::shared_ptr<SubscribeRequestDispatcherType> sharedDispatcher(
		shared_from_this(), &subscribeRequests);
	Details::UIEventConnection connection {std::move(sharedDispatcher), child};
	
	subscribeRequests.AddChild(std::move(child));
	return std::move(connection);
}
//-----------------------------------------------------------------------
void UIEventDispatcher::UpdateAnimation(DurationSeconds const& frameDuration)
{
	for (auto & node: children)
	{
		POMDOG_ASSERT(node);
		node->UpdateAnimation(frameDuration);
	}
}
//-----------------------------------------------------------------------
Optional<UI::PointerMouseEvent> UIEventDispatcher::FindPointerMouseEvent(MouseState const& mouseState) const
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
ButtonState UIEventDispatcher::CheckMouseButton(MouseState const& mouseState, UI::PointerMouseEvent const& pointerMouseEvent) const
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
void UIEventDispatcher::Sort()
{
	std::sort(std::begin(children), std::end(children),
		[](std::shared_ptr<UIView> & a, std::shared_ptr<UIView> & b) {
			return a->GlobalDrawOrder() < b->GlobalDrawOrder();
		});
}
//-----------------------------------------------------------------------
}// namespace UI
}// namespace Pomdog
