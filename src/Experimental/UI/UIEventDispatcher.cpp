// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/UI/UIEventDispatcher.hpp"
#include "Pomdog/Experimental/UI/UIHelper.hpp"
#include "Pomdog/Input/KeyboardState.hpp"
#include "Pomdog/Signals/detail/SignalBody.hpp"
#include <algorithm>

#include "Pomdog/Input/Keys.hpp"
#include "Pomdog/Logging/Log.hpp"

namespace Pomdog::UI {
namespace {

constexpr double DoubleClickTimeInterval = 0.500;

bool Intersects(const Point2D& position, UIElement& element)
{
    auto bounds = element.GetBounds();
    auto positionInChild = UIHelper::ConvertToChildSpace(position, element.GetGlobalTransform());
    return bounds.Contains(positionInChild);
}

std::shared_ptr<UIElement>
Find(
    const Point2D& position,
    const std::vector<std::weak_ptr<UIElement>>& children)
{
    for (auto& weakChild : children) {
        auto child = weakChild.lock();
        if (child && Intersects(position, *child)) {
            return child;
        }
    }
    return {};
}

class UIEventConnection final : public Pomdog::Detail::Signals::ConnectionBody {
private:
    std::weak_ptr<UIEventDispatcher> weakDispatcher;
    std::weak_ptr<UIElement> weakListener;

public:
    UIEventConnection(
        const std::weak_ptr<UIEventDispatcher>& weakDispatcherIn,
        const std::weak_ptr<UIElement>& weakListenerIn)
        : weakDispatcher(weakDispatcherIn)
        , weakListener(weakListenerIn)
    {
        POMDOG_ASSERT(!weakDispatcher.expired());
        POMDOG_ASSERT(!weakListener.expired());
    }

    void Disconnect() override
    {
        if (weakListener.expired()) {
            return;
        }

        if (auto dispatcher = weakDispatcher.lock()) {
            dispatcher->RemoveChild(weakListener);
        }

        weakDispatcher.reset();
        weakListener.reset();
    }

    [[nodiscard]] bool Valid() const override
    {
        if (weakListener.expired()) {
            return false;
        }
        if (auto p = weakDispatcher.lock(); p != nullptr) {
            return p->Contains(weakListener);
        }
        return false;
    }

    [[nodiscard]] std::unique_ptr<ConnectionBody> DeepCopy() const override
    {
        auto p = std::make_unique<UIEventConnection>(this->weakDispatcher, this->weakListener);
        return p;
    }
};

} // namespace

UIEventDispatcher::UIEventDispatcher(
    const std::shared_ptr<GameWindow>& windowIn,
    const std::shared_ptr<Keyboard>& keyboardIn)
    : window(windowIn)
    , keyboard(keyboardIn)
{
    keyDownConn = keyboard->KeyDown.Connect([this](Keys key) {
        if (auto element = focusedElement.lock(); element != nullptr) {
            element->OnKeyDown(keyboard->GetState(), key);
        }
    });

    keyUpConn = keyboard->KeyUp.Connect([this](Keys key) {
        if (auto element = focusedElement.lock(); element != nullptr) {
            element->OnKeyUp(keyboard->GetState(), key);
        }
    });

    textInputConn = keyboard->TextInput.Connect([this](const std::string& text) {
        if (auto element = focusedElement.lock(); element != nullptr) {
            element->OnTextInput(keyboard->GetState(), text);
        }
    });
}

Connection
UIEventDispatcher::Connect(const std::weak_ptr<UIElement>& child)
{
    auto body = std::make_unique<UIEventConnection>(shared_from_this(), child);
    auto connection = Connection{std::move(body)};
    subscribeRequests.push_back(child);

    return connection;
}

bool UIEventDispatcher::Contains(const std::weak_ptr<UIElement>& child) const
{
    auto f = [&](const std::weak_ptr<UIElement>& p) { return p.lock() == child.lock(); };

    if (auto iter = std::find_if(std::begin(children), std::end(children), f); iter != std::end(children)) {
        return !iter->expired();
    }

    auto iter = std::find_if(std::begin(subscribeRequests), std::end(subscribeRequests), f);
    if (iter != std::end(subscribeRequests)) {
        return !iter->expired();
    }
    return false;
}

void UIEventDispatcher::AddChild(std::weak_ptr<UIElement>&& child)
{
    subscribeRequests.push_back(std::move(child));
}

void UIEventDispatcher::RemoveChild(const std::weak_ptr<UIElement>& child)
{
    if (child.expired()) {
        return;
    }

    auto f = [&](const std::weak_ptr<UIElement>& p) { return p.lock() == child.lock(); };

    if (auto iter = std::find_if(std::begin(children), std::end(children), f); iter != std::end(children)) {
        iter->reset();
        POMDOG_ASSERT(iter->expired());
        return;
    }

    auto iter = std::find_if(std::begin(subscribeRequests), std::end(subscribeRequests), f);
    if (iter != std::end(subscribeRequests)) {
        iter->reset();
        POMDOG_ASSERT(iter->expired());
        return;
    }
}

void UIEventDispatcher::UpdateChildren()
{
    if (!subscribeRequests.empty()) {
        if (children.empty()) {
            std::swap(children, subscribeRequests);
        }
        else {
            children.reserve(children.size() + subscribeRequests.size());
            children.insert(std::end(children), std::begin(subscribeRequests), std::end(subscribeRequests));
            subscribeRequests.clear();
        }
        POMDOG_ASSERT(subscribeRequests.empty());
    }

    auto f = [](const std::weak_ptr<UIElement>& p) { return p.expired(); };
    children.erase(
        std::remove_if(std::begin(children), std::end(children), f),
        std::end(children));

    std::stable_sort(std::begin(children), std::end(children),
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

    POMDOG_ASSERT(std::end(children) == std::unique(std::begin(children), std::end(children),
        [](const std::weak_ptr<UIElement>& a, const std::weak_ptr<UIElement>& b) { return a.lock() == b.lock(); }));
}

void UIEventDispatcher::Touch(const MouseState& mouseState)
{
    auto const position = mouseState.Position;

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
    case PointerEventType::Entered: {
        auto node = Find(position, children);
        auto oldFocusedElement = pointerState->focusedElement.lock();

        if (!node || node != oldFocusedElement) {
            PointerExited(position);
            POMDOG_ASSERT(!pointerState);
        }
        else if (auto pointerMouseEvent = FindPointerMouseEvent(mouseState); pointerMouseEvent != std::nullopt) {
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

    if (pointerState) {
        auto focused = pointerState->focusedElement.lock();
        if (!focused) {
            pointerState.reset();
        }
        else {
            auto oldMouseWheelDelta = pointerState->pointerPoint.MouseWheelDelta;
            pointerState->pointerPoint.MouseWheelDelta = mouseState.ScrollWheel - pointerState->PrevScrollWheel;
            pointerState->PrevScrollWheel = mouseState.ScrollWheel;

            if (oldMouseWheelDelta != pointerState->pointerPoint.MouseWheelDelta) {
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

    auto element = pointerState->focusedElement.lock();
    element->OnPointerExited(pointerState->pointerPoint);
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

    auto element = pointerState->focusedElement.lock();
    element->OnPointerPressed(pointerState->pointerPoint);

    if (auto oldFocused = focusedElement.lock(); oldFocused != nullptr) {
        if (oldFocused != element) {
            oldFocused->OnFocusOut();
            focusedElement = element;
            element->OnFocusIn();
        }
    }
    else {
        POMDOG_ASSERT(focusedElement.expired());
        focusedElement = element;
        element->OnFocusIn();
    }
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

    auto element = pointerState->focusedElement.lock();
    element->OnPointerMoved(pointerState->pointerPoint);
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

    auto element = pointerState->focusedElement.lock();
    element->OnPointerReleased(pointerState->pointerPoint);
}

void UIEventDispatcher::UpdateAnimation(const Duration& frameDuration)
{
    if ((pointerState != nullptr) && (pointerState->lastClickTime < Duration{DoubleClickTimeInterval})) {
        pointerState->lastClickTime += frameDuration;
    }

    for (auto& weakChild : children) {
        if (auto child = weakChild.lock()) {
            child->UpdateAnimation(frameDuration);
        }
    }
}

std::optional<UI::PointerMouseEvent>
UIEventDispatcher::FindPointerMouseEvent(const MouseState& mouseState) const
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

ButtonState
UIEventDispatcher::CheckMouseButton(
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

} // namespace Pomdog::UI
