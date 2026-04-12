// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/ui_event_dispatcher.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/ui_helper.h"
#include "pomdog/input/keys.h"
#include "pomdog/logging/log.h"

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

void UIEventDispatcher::dispatchKeyDown(Keys key)
{
    if (auto widget = focusedWidget_.lock(); widget != nullptr) {
        widget->onKeyDown(*keyboard_, key);
    }
}

void UIEventDispatcher::dispatchKeyUp(Keys key)
{
    if (auto widget = focusedWidget_.lock(); widget != nullptr) {
        widget->onKeyUp(*keyboard_, key);
    }
}

void UIEventDispatcher::dispatchTextInput(const std::string& text)
{
    if (auto widget = focusedWidget_.lock(); widget != nullptr) {
        widget->onTextInput(*keyboard_, text);
    }
}

void UIEventDispatcher::dispatchTextInputFromKeyboard()
{
    POMDOG_ASSERT(keyboard_ != nullptr);
    const auto text = keyboard_->getTextInput();
    if (!text.empty()) {
        dispatchTextInput(std::string{text});
    }
}

void UIEventDispatcher::touch(const Point2D& position, const Mouse& mouse, std::vector<std::shared_ptr<Widget>>& children)
{

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
            else if (auto pointerMouseEvent = findPointerMouseEvent(mouse); pointerMouseEvent != std::nullopt) {
                pointerState_->pointerPoint.MouseEvent = pointerMouseEvent;
                POMDOG_ASSERT(checkMouseButton(mouse, *pointerMouseEvent) == ButtonState::Down);

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
            pointerEntered(position, mouse, node);
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
            checkMouseButton(mouse, *pointerState_->pointerPoint.MouseEvent) == ButtonState::Down) {
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
            pointerState_->pointerPoint.MouseWheelDelta = mouse.getScrollY() - pointerState_->prevScrollWheel;
            pointerState_->prevScrollWheel = mouse.getScrollY();

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
    const Mouse& mouse,
    const std::shared_ptr<Widget>& node)
{
    POMDOG_ASSERT(!pointerState_);
    pointerState_ = std::make_unique<PointerState>();

    pointerState_->pointerPoint.Event = PointerEventType::Entered;
    pointerState_->pointerPoint.Position = position;
    pointerState_->pointerPoint.ID = 0;
    pointerState_->pointerPoint.MouseWheelDelta = 0;
    pointerState_->prevScrollWheel = mouse.getScrollY();

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
UIEventDispatcher::findPointerMouseEvent(const Mouse& mouse) const
{
    if (mouse.isButtonDown(MouseButtons::Left)) {
        return PointerMouseEvent::LeftButtonPressed;
    }
    else if (mouse.isButtonDown(MouseButtons::Right)) {
        return PointerMouseEvent::RightButtonPressed;
    }
    else if (mouse.isButtonDown(MouseButtons::Middle)) {
        return PointerMouseEvent::MiddleButtonPressed;
    }
    else if (mouse.isButtonDown(MouseButtons::X1)) {
        return PointerMouseEvent::XButton1Pressed;
    }
    else if (mouse.isButtonDown(MouseButtons::X2)) {
        return PointerMouseEvent::XButton2Pressed;
    }
    return std::nullopt;
}

ButtonState
UIEventDispatcher::checkMouseButton(
    const Mouse& mouse,
    const PointerMouseEvent& pointerMouseEvent) const
{
    using pomdog::gui::PointerMouseEvent;
    switch (pointerMouseEvent) {
    case PointerMouseEvent::LeftButtonPressed:
        return mouse.isButtonDown(MouseButtons::Left) ? ButtonState::Down : ButtonState::Up;
    case PointerMouseEvent::MiddleButtonPressed:
        return mouse.isButtonDown(MouseButtons::Middle) ? ButtonState::Down : ButtonState::Up;
    case PointerMouseEvent::RightButtonPressed:
        return mouse.isButtonDown(MouseButtons::Right) ? ButtonState::Down : ButtonState::Up;
    case PointerMouseEvent::XButton1Pressed:
        return mouse.isButtonDown(MouseButtons::X1) ? ButtonState::Down : ButtonState::Up;
    case PointerMouseEvent::XButton2Pressed:
        return mouse.isButtonDown(MouseButtons::X2) ? ButtonState::Down : ButtonState::Up;
    case PointerMouseEvent::ScrollWheel:
        break;
    }
    return ButtonState::Up;
}

} // namespace pomdog::gui
