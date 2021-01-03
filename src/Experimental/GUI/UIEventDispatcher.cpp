// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/GUI/UIEventDispatcher.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/GUI/UIHelper.hpp"
#include "Pomdog/Input/KeyboardState.hpp"
#include "Pomdog/Input/Keys.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Signals/Detail/SignalBody.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::GUI {
namespace {

constexpr double DoubleClickTimeInterval = 0.500;

bool Intersects(const Point2D& position, Widget& widget)
{
    auto bounds = widget.GetBounds();
    auto parent = widget.GetParent();
    if (parent == nullptr) {
        return bounds.Contains(position);
    }
    auto positionInChild = UIHelper::ProjectToChildSpace(position, parent->GetGlobalPosition());
    return bounds.Contains(positionInChild);
}

std::shared_ptr<Widget>
FindTraversal(
    const Point2D& position,
    const std::shared_ptr<Widget>& widget,
    const std::function<bool(const std::shared_ptr<Widget>&)>& func)
{
    POMDOG_ASSERT(func != nullptr);

    auto positionInChild = UIHelper::ProjectToChildSpace(position, widget->GetGlobalPosition());

    auto child = widget->GetChildAt(positionInChild);
    if (child != nullptr) {
        auto p = FindTraversal(position, child, func);
        if ((p != nullptr) && func(p)) {
            return p;
        }
    }
    return child;
}

std::shared_ptr<Widget>
Find(
    const Point2D& position,
    const std::vector<std::shared_ptr<Widget>>& widgets,
    const std::function<bool(const std::shared_ptr<Widget>&)>& func)
{
    POMDOG_ASSERT(func != nullptr);

    for (const auto& widget : widgets) {
        if ((widget == nullptr) || !widget->IsVisible()) {
            continue;
        }

        if (!Intersects(position, *widget)) {
            continue;
        }

        auto child = FindTraversal(position, widget, func);
        if (child != nullptr) {
            return child;
        }
        if (widget->IsInteractable()) {
            return widget;
        }
        continue;
    }
    return nullptr;
}

std::shared_ptr<Widget>
Find(
    const Point2D& position,
    const std::vector<std::shared_ptr<Widget>>& widgets)
{
    const auto func = [](const std::shared_ptr<Widget>& p) -> bool {
        return p->IsInteractable();
    };

    return Find(position, widgets, func);
}

} // namespace

UIEventDispatcher::UIEventDispatcher(
    const std::shared_ptr<GameWindow>& windowIn,
    const std::shared_ptr<Keyboard>& keyboardIn)
    : window(windowIn)
    , keyboard(keyboardIn)
{
    keyDownConn = keyboard->KeyDown.Connect([this](Keys key) {
        if (auto widget = focusedWidget.lock(); widget != nullptr) {
            widget->OnKeyDown(keyboard->GetState(), key);
        }
    });

    keyUpConn = keyboard->KeyUp.Connect([this](Keys key) {
        if (auto widget = focusedWidget.lock(); widget != nullptr) {
            widget->OnKeyUp(keyboard->GetState(), key);
        }
    });

    textInputConn = keyboard->TextInput.Connect([this](const std::string& text) {
        if (auto widget = focusedWidget.lock(); widget != nullptr) {
            widget->OnTextInput(keyboard->GetState(), text);
        }
    });
}

void UIEventDispatcher::SetFocusWidget(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(widget != nullptr);

    if (auto oldFocused = focusedWidget.lock(); oldFocused != nullptr) {
        oldFocused->OnFocusOut();
        focusedWidget.reset();
    }

    focusedWidget = widget;
    widget->OnFocusIn();

    FocusChanged(widget);
}

void UIEventDispatcher::ClearFocus(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(widget != nullptr);

    if (auto oldFocused = focusedWidget.lock(); oldFocused != nullptr) {
        if (oldFocused == widget) {
            oldFocused->OnFocusOut();
            focusedWidget.reset();
        }
    }
}

void UIEventDispatcher::Touch(const MouseState& mouseState, std::vector<std::shared_ptr<Widget>>& children)
{
    auto const position = mouseState.Position;

    if (pointerState) {
        switch (pointerState->pointerPoint.Event) {
        case PointerEventType::Released:
        case PointerEventType::Entered: {
            auto node = Find(position, children);
            auto oldFocusedWidget = pointerState->focusedWidget.lock();

            if (!node || node != oldFocusedWidget) {
                PointerExited(position);
                POMDOG_ASSERT(!pointerState);
            }
            else if (auto pointerMouseEvent = FindPointerMouseEvent(mouseState); pointerMouseEvent != std::nullopt) {
                pointerState->pointerPoint.MouseEvent = pointerMouseEvent;
                POMDOG_ASSERT(CheckMouseButton(mouseState, *pointerMouseEvent) == ButtonState::Pressed);

                POMDOG_ASSERT(node == oldFocusedWidget);
                PointerPressed(position);
            }
            break;
        }
        case PointerEventType::Canceled:
            break;
        case PointerEventType::CaptureLost:
            break;
        case PointerEventType::Exited:
            break;
        case PointerEventType::Pressed:
            break;
        case PointerEventType::Moved:
            break;
        case PointerEventType::WheelChanged:
            break;
        }
    }

    if (!pointerState) {
        if (auto node = Find(position, children); node != nullptr) {
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
        break;
    case PointerEventType::Entered:
        break;
    case PointerEventType::Exited:
        break;
    case PointerEventType::Pressed:
    case PointerEventType::Moved:
        if (pointerState->pointerPoint.MouseEvent &&
            CheckMouseButton(mouseState, *pointerState->pointerPoint.MouseEvent) == ButtonState::Pressed) {
            PointerMoved(position);
        }
        else {
            PointerReleased(position);
        }
        break;
    case PointerEventType::WheelChanged:
        break;
    }

    if (pointerState != nullptr) {
        const auto func = [](const std::shared_ptr<Widget>& p) -> bool {
            return p->IsInteractable() && p->IsWheelFocusEnabled();
        };

        auto wheelFocusChild = Find(position, children, func);

        if (wheelFocusChild != nullptr) {
            auto oldMouseWheelDelta = pointerState->pointerPoint.MouseWheelDelta;
            pointerState->pointerPoint.MouseWheelDelta = mouseState.ScrollWheel - pointerState->PrevScrollWheel;
            pointerState->PrevScrollWheel = mouseState.ScrollWheel;

            if (oldMouseWheelDelta != pointerState->pointerPoint.MouseWheelDelta) {
                POMDOG_ASSERT(wheelFocusChild != nullptr);
                wheelFocusChild->OnPointerWheelChanged(pointerState->pointerPoint);
            }
        }

        if (pointerState->focusedWidget.expired()) {
            pointerState.reset();
        }
    }
}

void UIEventDispatcher::PointerEntered(
    const Point2D& position,
    const MouseState& mouseState,
    const std::shared_ptr<Widget>& node)
{
    POMDOG_ASSERT(!pointerState);
    pointerState = std::make_unique<PointerState>();

    pointerState->pointerPoint.Event = PointerEventType::Entered;
    pointerState->pointerPoint.Position = position;
    pointerState->pointerPoint.ID = 0;
    pointerState->pointerPoint.MouseWheelDelta = 0;
    pointerState->PrevScrollWheel = mouseState.ScrollWheel;

    node->OnPointerEntered(pointerState->pointerPoint);
    if (node->GetCurrentCursor()) {
        window->SetMouseCursor(*node->GetCurrentCursor());
    }
    else {
        window->SetMouseCursor(MouseCursor::Arrow);
    }

    pointerState->focusedWidget = node;
}

void UIEventDispatcher::PointerExited(const Point2D& position)
{
    POMDOG_ASSERT(pointerState);

    if (pointerState->focusedWidget.expired()) {
        pointerState.reset();
        return;
    }

    pointerState->pointerPoint.Event = PointerEventType::Exited;
    pointerState->pointerPoint.Position = position;

    auto widget = pointerState->focusedWidget.lock();
    widget->OnPointerExited(pointerState->pointerPoint);
    pointerState.reset();

    window->SetMouseCursor(MouseCursor::Arrow);
}

void UIEventDispatcher::PointerPressed(const Point2D& position)
{
    POMDOG_ASSERT(pointerState);

    if (pointerState->focusedWidget.expired()) {
        pointerState.reset();
        return;
    }

    pointerState->pointerPoint.Event = PointerEventType::Pressed;
    pointerState->pointerPoint.Position = position;
    pointerState->pointerPoint.ID = 0;

    if ((pointerState->lastClickPosition == position) &&
        (pointerState->lastClickTime <= Duration{DoubleClickTimeInterval})) {
        pointerState->pointerPoint.ClickCount += 1;
        if (pointerState->pointerPoint.ClickCount > 2) {
            pointerState->pointerPoint.ClickCount = 1;
        }
    }
    else {
        pointerState->pointerPoint.ClickCount = 1;
        pointerState->lastClickPosition = position;
    }
    pointerState->lastClickTime = Duration::zero();

    auto widget = pointerState->focusedWidget.lock();
    widget->OnPointerPressed(pointerState->pointerPoint);

    if (auto oldFocused = focusedWidget.lock(); oldFocused != nullptr) {
        if (oldFocused != widget) {
            oldFocused->OnFocusOut();
            focusedWidget = widget;
            widget->OnFocusIn();
        }
    }
    else {
        POMDOG_ASSERT(focusedWidget.expired());
        focusedWidget = widget;
        widget->OnFocusIn();
    }
    FocusChanged(widget);
}

void UIEventDispatcher::PointerMoved(const Point2D& position)
{
    POMDOG_ASSERT(pointerState);

    if (pointerState->focusedWidget.expired()) {
        pointerState.reset();
        return;
    }

    pointerState->pointerPoint.Event = PointerEventType::Moved;
    pointerState->pointerPoint.Position = position;

    auto widget = pointerState->focusedWidget.lock();
    widget->OnPointerMoved(pointerState->pointerPoint);
}

void UIEventDispatcher::PointerReleased(const Point2D& position)
{
    POMDOG_ASSERT(pointerState);

    if (pointerState->focusedWidget.expired()) {
        pointerState.reset();
        return;
    }

    pointerState->pointerPoint.Event = PointerEventType::Released;
    pointerState->pointerPoint.Position = position;

    auto widget = pointerState->focusedWidget.lock();
    widget->OnPointerReleased(pointerState->pointerPoint);
}

void UIEventDispatcher::UpdateAnimation(const Duration& frameDuration)
{
    if ((pointerState != nullptr) && (pointerState->lastClickTime < Duration{DoubleClickTimeInterval})) {
        pointerState->lastClickTime += frameDuration;
    }
}

std::shared_ptr<Widget> UIEventDispatcher::GetFocusWidget() const
{
    return focusedWidget.lock();
}

std::optional<PointerMouseEvent>
UIEventDispatcher::FindPointerMouseEvent(const MouseState& mouseState) const
{
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

ButtonState
UIEventDispatcher::CheckMouseButton(
    const MouseState& mouseState,
    const PointerMouseEvent& pointerMouseEvent) const
{
    using Pomdog::GUI::PointerMouseEvent;
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
    case PointerMouseEvent::ScrollWheel:
        break;
    }
    return ButtonState::Released;
}

} // namespace Pomdog::GUI
