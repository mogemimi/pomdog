// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/input/backends/keyboard_state.h"
#include "pomdog/input/keys.h"
#include "pomdog/logging/log.h"
#include "pomdog/signals/detail/signal_body.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {
namespace {

constexpr double DoubleClickTimeInterval = 0.500;

[[nodiscard]] bool
intersects(const Point2D& position, Widget& widget)
{
    const auto bounds = widget.getBounds();
    const auto parent = widget.getParent();
    if (parent == nullptr) {
        return bounds.contains(position);
    }
    auto positionInChild = UIHelper::projectToChildSpace(position, parent->getGlobalPosition());
    return bounds.contains(positionInChild);
}

[[nodiscard]] std::shared_ptr<Widget>
findTraversal(
    const Point2D& position,
    const std::shared_ptr<Widget>& widget,
    const std::function<bool(const std::shared_ptr<Widget>&)>& func)
{
    POMDOG_ASSERT(func != nullptr);

    auto positionInChild = UIHelper::projectToChildSpace(position, widget->getGlobalPosition());

    auto child = widget->getChildAt(positionInChild);
    if (child != nullptr) {
        auto p = findTraversal(position, child, func);
        if ((p != nullptr) && func(p)) {
            return p;
        }
    }
    return child;
}

[[nodiscard]] std::shared_ptr<Widget>
find(
    const Point2D& position,
    const std::vector<std::shared_ptr<Widget>>& widgets,
    const std::function<bool(const std::shared_ptr<Widget>&)>& func)
{
    POMDOG_ASSERT(func != nullptr);

    for (const auto& widget : widgets) {
        if ((widget == nullptr) || !widget->isVisible()) {
            continue;
        }

        if (!intersects(position, *widget)) {
            continue;
        }

        auto child = findTraversal(position, widget, func);
        if (child != nullptr) {
            return child;
        }
        if (widget->isInteractable()) {
            return widget;
        }
        continue;
    }
    return nullptr;
}

[[nodiscard]] std::shared_ptr<Widget>
find(
    const Point2D& position,
    const std::vector<std::shared_ptr<Widget>>& widgets)
{
    const auto func = [](const std::shared_ptr<Widget>& p) -> bool {
        return p->isInteractable();
    };

    return find(position, widgets, func);
}

} // namespace

UIEventDispatcher::UIEventDispatcher(
    const std::shared_ptr<GameWindow>& windowIn,
    const std::shared_ptr<Keyboard>& keyboardIn)
    : window_(windowIn)
    , keyboard_(keyboardIn)
{
    keyDownConn_ = keyboard_->KeyDown.connect([this](Keys key) {
        if (auto widget = focusedWidget_.lock(); widget != nullptr) {
            widget->onKeyDown(keyboard_->getState(), key);
        }
    });

    keyUpConn_ = keyboard_->KeyUp.connect([this](Keys key) {
        if (auto widget = focusedWidget_.lock(); widget != nullptr) {
            widget->onKeyUp(keyboard_->getState(), key);
        }
    });

    textInputConn_ = keyboard_->TextInput.connect([this](const std::string& text) {
        if (auto widget = focusedWidget_.lock(); widget != nullptr) {
            widget->onTextInput(keyboard_->getState(), text);
        }
    });
}

void UIEventDispatcher::setFocusWidget(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(widget != nullptr);

    if (auto oldFocused = focusedWidget_.lock(); oldFocused != nullptr) {
        oldFocused->onFocusOut();
        focusedWidget_.reset();
    }

    focusedWidget_ = widget;
    widget->onFocusIn();

    FocusChanged(widget);
}

void UIEventDispatcher::clearFocus(const std::shared_ptr<Widget>& widget)
{
    POMDOG_ASSERT(widget != nullptr);

    if (auto oldFocused = focusedWidget_.lock(); oldFocused != nullptr) {
        if (oldFocused == widget) {
            oldFocused->onFocusOut();
            focusedWidget_.reset();
        }
    }
}

void UIEventDispatcher::touch(const MouseState& mouseState, std::vector<std::shared_ptr<Widget>>& children)
{
    auto const position = mouseState.position;

    if (pointerState_) {
        switch (pointerState_->pointerPoint.Event) {
        case PointerEventType::Released:
        case PointerEventType::Entered: {
            auto node = find(position, children);
            auto oldFocusedWidget = pointerState_->focusedWidget_.lock();

            if (!node || node != oldFocusedWidget) {
                pointerExited(position);
                POMDOG_ASSERT(!pointerState_);
            }
            else if (auto pointerMouseEvent = findPointerMouseEvent(mouseState); pointerMouseEvent != std::nullopt) {
                pointerState_->pointerPoint.MouseEvent = pointerMouseEvent;
                POMDOG_ASSERT(checkMouseButton(mouseState, *pointerMouseEvent) == ButtonState::Down);

                POMDOG_ASSERT(node == oldFocusedWidget);
                pointerPressed(position);
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

    if (!pointerState_) {
        if (auto node = find(position, children); node != nullptr) {
            POMDOG_ASSERT(!pointerState_);
            pointerEntered(position, mouseState, node);
        }
    }

    if (!pointerState_) {
        return;
    }

    switch (pointerState_->pointerPoint.Event) {
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
        if (pointerState_->pointerPoint.MouseEvent &&
            checkMouseButton(mouseState, *pointerState_->pointerPoint.MouseEvent) == ButtonState::Down) {
            pointerMoved(position);
        }
        else {
            pointerReleased(position);
        }
        break;
    case PointerEventType::WheelChanged:
        break;
    }

    if (pointerState_ != nullptr) {
        const auto func = [](const std::shared_ptr<Widget>& p) -> bool {
            return p->isInteractable() && p->isWheelFocusEnabled();
        };

        auto wheelFocusChild = find(position, children, func);

        if (wheelFocusChild != nullptr) {
            auto oldMouseWheelDelta = pointerState_->pointerPoint.MouseWheelDelta;
            pointerState_->pointerPoint.MouseWheelDelta = mouseState.scrollWheel - pointerState_->prevScrollWheel;
            pointerState_->prevScrollWheel = mouseState.scrollWheel;

            if (oldMouseWheelDelta != pointerState_->pointerPoint.MouseWheelDelta) {
                POMDOG_ASSERT(wheelFocusChild != nullptr);
                wheelFocusChild->onPointerWheelChanged(pointerState_->pointerPoint);
            }
        }

        if (pointerState_->focusedWidget_.expired()) {
            pointerState_.reset();
        }
    }
}

void UIEventDispatcher::pointerEntered(
    const Point2D& position,
    const MouseState& mouseState,
    const std::shared_ptr<Widget>& node)
{
    POMDOG_ASSERT(!pointerState_);
    pointerState_ = std::make_unique<PointerState>();

    pointerState_->pointerPoint.Event = PointerEventType::Entered;
    pointerState_->pointerPoint.Position = position;
    pointerState_->pointerPoint.ID = 0;
    pointerState_->pointerPoint.MouseWheelDelta = 0;
    pointerState_->prevScrollWheel = mouseState.scrollWheel;

    node->onPointerEntered(pointerState_->pointerPoint);
    if (node->getCurrentCursor()) {
        window_->setMouseCursor(*node->getCurrentCursor());
    }
    else {
        window_->setMouseCursor(MouseCursor::Arrow);
    }

    pointerState_->focusedWidget_ = node;
}

void UIEventDispatcher::pointerExited(const Point2D& position)
{
    POMDOG_ASSERT(pointerState_);

    if (pointerState_->focusedWidget_.expired()) {
        pointerState_.reset();
        return;
    }

    pointerState_->pointerPoint.Event = PointerEventType::Exited;
    pointerState_->pointerPoint.Position = position;

    auto widget = pointerState_->focusedWidget_.lock();
    widget->onPointerExited(pointerState_->pointerPoint);
    pointerState_.reset();

    window_->setMouseCursor(MouseCursor::Arrow);
}

void UIEventDispatcher::pointerPressed(const Point2D& position)
{
    POMDOG_ASSERT(pointerState_);

    if (pointerState_->focusedWidget_.expired()) {
        pointerState_.reset();
        return;
    }

    pointerState_->pointerPoint.Event = PointerEventType::Pressed;
    pointerState_->pointerPoint.Position = position;
    pointerState_->pointerPoint.ID = 0;

    if ((pointerState_->lastClickPosition == position) &&
        (pointerState_->lastClickTime <= Duration{DoubleClickTimeInterval})) {
        pointerState_->pointerPoint.ClickCount += 1;
        if (pointerState_->pointerPoint.ClickCount > 2) {
            pointerState_->pointerPoint.ClickCount = 1;
        }
    }
    else {
        pointerState_->pointerPoint.ClickCount = 1;
        pointerState_->lastClickPosition = position;
    }
    pointerState_->lastClickTime = Duration::zero();

    auto widget = pointerState_->focusedWidget_.lock();
    widget->onPointerPressed(pointerState_->pointerPoint);

    if (auto oldFocused = focusedWidget_.lock(); oldFocused != nullptr) {
        if (oldFocused != widget) {
            oldFocused->onFocusOut();
            focusedWidget_ = widget;
            widget->onFocusIn();
        }
    }
    else {
        POMDOG_ASSERT(focusedWidget_.expired());
        focusedWidget_ = widget;
        widget->onFocusIn();
    }
    FocusChanged(widget);
}

void UIEventDispatcher::pointerMoved(const Point2D& position)
{
    POMDOG_ASSERT(pointerState_);

    if (pointerState_->focusedWidget_.expired()) {
        pointerState_.reset();
        return;
    }

    pointerState_->pointerPoint.Event = PointerEventType::Moved;
    pointerState_->pointerPoint.Position = position;

    auto widget = pointerState_->focusedWidget_.lock();
    widget->onPointerMoved(pointerState_->pointerPoint);
}

void UIEventDispatcher::pointerReleased(const Point2D& position)
{
    POMDOG_ASSERT(pointerState_);

    if (pointerState_->focusedWidget_.expired()) {
        pointerState_.reset();
        return;
    }

    pointerState_->pointerPoint.Event = PointerEventType::Released;
    pointerState_->pointerPoint.Position = position;

    auto widget = pointerState_->focusedWidget_.lock();
    widget->onPointerReleased(pointerState_->pointerPoint);
}

void UIEventDispatcher::updateAnimation(const Duration& frameDuration)
{
    if ((pointerState_ != nullptr) && (pointerState_->lastClickTime < Duration{DoubleClickTimeInterval})) {
        pointerState_->lastClickTime += frameDuration;
    }
}

std::shared_ptr<Widget> UIEventDispatcher::getFocusWidget() const
{
    return focusedWidget_.lock();
}

std::optional<PointerMouseEvent>
UIEventDispatcher::findPointerMouseEvent(const MouseState& mouseState) const
{
    if (mouseState.leftButton == ButtonState::Down) {
        return PointerMouseEvent::LeftButtonPressed;
    }
    else if (mouseState.rightButton == ButtonState::Down) {
        return PointerMouseEvent::RightButtonPressed;
    }
    else if (mouseState.middleButton == ButtonState::Down) {
        return PointerMouseEvent::MiddleButtonPressed;
    }
    else if (mouseState.xButton1 == ButtonState::Down) {
        return PointerMouseEvent::XButton1Pressed;
    }
    else if (mouseState.xButton2 == ButtonState::Down) {
        return PointerMouseEvent::XButton2Pressed;
    }
    return std::nullopt;
}

ButtonState
UIEventDispatcher::checkMouseButton(
    const MouseState& mouseState,
    const PointerMouseEvent& pointerMouseEvent) const
{
    using pomdog::gui::PointerMouseEvent;
    switch (pointerMouseEvent) {
    case PointerMouseEvent::LeftButtonPressed:
        return mouseState.leftButton;
    case PointerMouseEvent::MiddleButtonPressed:
        return mouseState.middleButton;
    case PointerMouseEvent::RightButtonPressed:
        return mouseState.rightButton;
    case PointerMouseEvent::XButton1Pressed:
        return mouseState.xButton1;
    case PointerMouseEvent::XButton2Pressed:
        return mouseState.xButton2;
    case PointerMouseEvent::ScrollWheel:
        break;
    }
    return ButtonState::Up;
}

} // namespace pomdog::gui
