// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include "UIEventDispatcher.hpp"
#include "UIHelper.hpp"
#include "detail/UIEventConnection.hpp"
#include <algorithm>

namespace Pomdog {
namespace UI {
namespace {

static bool Intersects(Point2D const& position, UIElement & element)
{
    auto bounds = element.Bounds();
    auto positionInChild = UIHelper::ConvertToChildSpace(position, element.GlobalTransform());
    return bounds.Contains(positionInChild);
}
//-----------------------------------------------------------------------
static std::shared_ptr<UIElement> Find(Point2D const& position,
    std::vector<std::weak_ptr<UIElement>> const& children)
{
    for (auto & weakChild: children)
    {
        auto child = weakChild.lock();
        if (child && Intersects(position, *child)) {
            return child;
        }
    }
    return {};
}

} // unnamed namespace
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
        for (auto & weakChild: subscribeRequests.Removed)
        {
            if (weakChild.expired()) {
                continue;
            }

            auto child = weakChild.lock();
            auto f = [&child](std::weak_ptr<UIElement> const& p) {
                return p.lock() == child;
            };

            children.erase(std::remove_if(std::begin(children), std::end(children), f), std::end(children));
        }
    }

    children.erase(std::remove_if(std::begin(children), std::end(children),
        [](std::weak_ptr<UIElement> const& p) { return p.expired(); }),
        std::end(children));

    Sort();
    POMDOG_ASSERT(std::end(children) == std::unique(std::begin(children), std::end(children),
        [](std::weak_ptr<UIElement> const& a, std::weak_ptr<UIElement> const& b){ return a.lock() == b.lock(); }));
}
//-----------------------------------------------------------------------
void UIEventDispatcher::Touch(MouseState const& mouseState)
{
    auto const position = mouseState.Position;

    if (!pointerState)
    {
        if (auto node = Find(position, children))
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
//-----------------------------------------------------------------------
void UIEventDispatcher::PointerEntered(Point2D const& position,
    MouseState const& mouseState, std::shared_ptr<UIElement> const& node)
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
//-----------------------------------------------------------------------
void UIEventDispatcher::PointerPressed(Point2D const& position)
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
//-----------------------------------------------------------------------
void UIEventDispatcher::PointerMoved(Point2D const& position)
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
//-----------------------------------------------------------------------
void UIEventDispatcher::PointerReleased(Point2D const& position)
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
//-----------------------------------------------------------------------
Detail::UIEventConnection UIEventDispatcher::Connect(std::weak_ptr<UIElement> const& child)
{
    std::shared_ptr<SubscribeRequestDispatcherType> sharedDispatcher(
        shared_from_this(), &subscribeRequests);

    Detail::UIEventConnection connection {std::move(sharedDispatcher), child};
    subscribeRequests.AddChild(child);

    return std::move(connection);
}
//-----------------------------------------------------------------------
void UIEventDispatcher::UpdateAnimation(Duration const& frameDuration)
{
    for (auto & weakChild: children)
    {
        if (auto child = weakChild.lock()) {
            child->UpdateAnimation(frameDuration);
        }
    }
}
//-----------------------------------------------------------------------
Optional<UI::PointerMouseEvent> UIEventDispatcher::FindPointerMouseEvent(
    MouseState const& mouseState) const
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
    return Pomdog::NullOpt;
}
//-----------------------------------------------------------------------
ButtonState UIEventDispatcher::CheckMouseButton(MouseState const& mouseState,
    UI::PointerMouseEvent const& pointerMouseEvent) const
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
    children.erase(std::remove_if(std::begin(children), std::end(children),
        [](std::weak_ptr<UIElement> const& a){ return a.expired(); }),
        std::end(children));

    std::sort(std::begin(children), std::end(children),
        [](std::weak_ptr<UIElement> const& a, std::weak_ptr<UIElement> const& b) {
            auto sharedA = a.lock();
            auto sharedB = b.lock();
            if (!sharedA) {
                return false;
            }
            if (!sharedB) {
                return true;
            }
            return sharedA->GlobalDrawOrder() < sharedB->GlobalDrawOrder();
        });
}
//-----------------------------------------------------------------------
} // namespace UI
} // namespace Pomdog
