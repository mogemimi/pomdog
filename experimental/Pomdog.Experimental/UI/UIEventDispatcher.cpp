// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/UI/UIEventDispatcher.hpp"
#include "Pomdog.Experimental/UI/UIHelper.hpp"
#include "Pomdog.Experimental/UI/detail/UIEventConnection.hpp"
#include <algorithm>

namespace Pomdog {
namespace UI {
namespace {

bool Intersects(const Point2D& position, UIElement & element)
{
    auto bounds = element.GetBounds();
    auto positionInChild = UIHelper::ConvertToChildSpace(position, element.GetGlobalTransform());
    return bounds.Contains(positionInChild);
}

std::shared_ptr<UIElement> Find(
    const Point2D& position,
    const std::vector<std::weak_ptr<UIElement>>& children)
{
    for (auto & weakChild : children) {
        auto child = weakChild.lock();
        if (child && Intersects(position, *child)) {
            return child;
        }
    }
    return {};
}

} // unnamed namespace

UIEventDispatcher::UIEventDispatcher(const std::shared_ptr<GameWindow>& windowIn)
    : window(windowIn)
{}

void UIEventDispatcher::UpdateChildren()
{
    if (!subscribeRequests.Added.empty()) {
        children.insert(std::end(children), std::begin(subscribeRequests.Added), std::end(subscribeRequests.Added));
        subscribeRequests.Added.clear();
    }

    if (!subscribeRequests.Removed.empty()) {
        for (auto & weakChild: subscribeRequests.Removed) {
            if (weakChild.expired()) {
                continue;
            }

            auto child = weakChild.lock();
            auto f = [&child](const std::weak_ptr<UIElement>& p) {
                return p.lock() == child;
            };

            children.erase(std::remove_if(std::begin(children), std::end(children), f), std::end(children));
        }
    }

    children.erase(std::remove_if(std::begin(children), std::end(children),
        [](const std::weak_ptr<UIElement>& p) { return p.expired(); }),
        std::end(children));

    Sort();
    POMDOG_ASSERT(std::end(children) == std::unique(std::begin(children), std::end(children),
        [](const std::weak_ptr<UIElement>& a, const std::weak_ptr<UIElement>& b){ return a.lock() == b.lock(); }));
}

void UIEventDispatcher::Touch(const MouseState& mouseState)
{
    auto const position = mouseState.Position;

    if (!pointerState) {
        if (auto node = Find(position, children)) {
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
        auto node = Find(position, children);
        auto oldFocusedElement = pointerState->focusedElement.lock();

        if (!node || node != oldFocusedElement) {
            PointerExited(position);
            POMDOG_ASSERT(!pointerState);
        }
        else if (auto pointerMouseEvent = FindPointerMouseEvent(mouseState)) {
            pointerState->pointerPoint.MouseEvent = pointerMouseEvent;
            POMDOG_ASSERT(CheckMouseButton(mouseState, *pointerMouseEvent) == ButtonState::Pressed);

            POMDOG_ASSERT(node == oldFocusedElement);
            POMDOG_ASSERT(Intersects(position, *oldFocusedElement));
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
        auto focused = pointerState->focusedElement.lock();
        if (!focused)
        {
            pointerState.reset();
        }
        else
        {
            auto oldMouseWheelDelta = pointerState->pointerPoint.MouseWheelDelta;
            pointerState->pointerPoint.MouseWheelDelta = mouseState.ScrollWheel - pointerState->PrevScrollWheel;
            pointerState->PrevScrollWheel = mouseState.ScrollWheel;

            if (oldMouseWheelDelta != pointerState->pointerPoint.MouseWheelDelta)
            {
                POMDOG_ASSERT(focused);
                focused->OnPointerWheelChanged(pointerState->pointerPoint);
            }
        }
    }
}

void UIEventDispatcher::PointerEntered(
    const Point2D& position,
    const MouseState& mouseState,
    const std::shared_ptr<UIElement>& node)
{
    POMDOG_ASSERT(!pointerState);
    pointerState = std::make_unique<PointerState>();

    pointerState->pointerPoint.Event = PointerEventType::Entered;
    pointerState->pointerPoint.Position = position;
    pointerState->pointerPoint.Id = 0;
    pointerState->pointerPoint.MouseWheelDelta = 0;
    pointerState->PrevScrollWheel = mouseState.ScrollWheel;

    node->OnPointerEntered(pointerState->pointerPoint);
    if (node->GetCurrentCursor()) {
        window->SetMouseCursor(*node->GetCurrentCursor());
    }
    else {
        window->SetMouseCursor(MouseCursor::Arrow);
    }

    pointerState->focusedElement = node;
}

void UIEventDispatcher::PointerExited(const Point2D& position)
{
    POMDOG_ASSERT(pointerState);

    if (pointerState->focusedElement.expired()) {
        pointerState.reset();
        return;
    }

    pointerState->pointerPoint.Event = PointerEventType::Exited;
    pointerState->pointerPoint.Position = position;

    auto focusedElement = pointerState->focusedElement.lock();
    focusedElement->OnPointerExited(pointerState->pointerPoint);
    pointerState.reset();

    window->SetMouseCursor(MouseCursor::Arrow);
}

void UIEventDispatcher::PointerPressed(const Point2D& position)
{
    POMDOG_ASSERT(pointerState);

    if (pointerState->focusedElement.expired()) {
        pointerState.reset();
        return;
    }

    pointerState->pointerPoint.Event = PointerEventType::Pressed;
    pointerState->pointerPoint.Position = position;
    pointerState->pointerPoint.Id = 0;

    auto focusedElement = pointerState->focusedElement.lock();
    focusedElement->OnPointerPressed(pointerState->pointerPoint);
}

void UIEventDispatcher::PointerMoved(const Point2D& position)
{
    POMDOG_ASSERT(pointerState);

    if (pointerState->focusedElement.expired()) {
        pointerState.reset();
        return;
    }

    pointerState->pointerPoint.Event = PointerEventType::Moved;
    pointerState->pointerPoint.Position = position;

    auto focusedElement = pointerState->focusedElement.lock();
    focusedElement->OnPointerMoved(pointerState->pointerPoint);
}

void UIEventDispatcher::PointerReleased(const Point2D& position)
{
    POMDOG_ASSERT(pointerState);

    if (pointerState->focusedElement.expired()) {
        pointerState.reset();
        return;
    }

    pointerState->pointerPoint.Event = PointerEventType::Released;
    pointerState->pointerPoint.Position = position;

    auto focusedElement = pointerState->focusedElement.lock();
    focusedElement->OnPointerReleased(pointerState->pointerPoint);
}

Detail::UIEventConnection UIEventDispatcher::Connect(const std::weak_ptr<UIElement>& child)
{
    std::shared_ptr<SubscribeRequestDispatcherType> sharedDispatcher(
        shared_from_this(), &subscribeRequests);

    Detail::UIEventConnection connection {std::move(sharedDispatcher), child};
    subscribeRequests.AddChild(child);

    return connection;
}

void UIEventDispatcher::UpdateAnimation(const Duration& frameDuration)
{
    for (auto & weakChild : children) {
        if (auto child = weakChild.lock()) {
            child->UpdateAnimation(frameDuration);
        }
    }
}

std::optional<UI::PointerMouseEvent> UIEventDispatcher::FindPointerMouseEvent(
    const MouseState& mouseState) const
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
    return std::nullopt;
}

ButtonState UIEventDispatcher::CheckMouseButton(
    const MouseState& mouseState,
    const UI::PointerMouseEvent& pointerMouseEvent) const
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

void UIEventDispatcher::Sort()
{
    children.erase(std::remove_if(std::begin(children), std::end(children),
        [](const std::weak_ptr<UIElement>& a){ return a.expired(); }),
        std::end(children));

    std::sort(std::begin(children), std::end(children),
        [](const std::weak_ptr<UIElement>& a, const std::weak_ptr<UIElement>& b) {
            auto sharedA = a.lock();
            auto sharedB = b.lock();
            if (!sharedA) {
                return false;
            }
            if (!sharedB) {
                return true;
            }
            return sharedA->GetGlobalDrawOrder() < sharedB->GetGlobalDrawOrder();
        });
}

} // namespace UI
} // namespace Pomdog
