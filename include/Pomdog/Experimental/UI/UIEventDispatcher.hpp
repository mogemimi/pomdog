// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/GameWindow.hpp"
#include "Pomdog/Experimental/UI/PointerEventType.hpp"
#include "Pomdog/Experimental/UI/PointerPoint.hpp"
#include "Pomdog/Experimental/UI/UIElement.hpp"
#include "Pomdog/Input/Keyboard.hpp"
#include "Pomdog/Input/MouseState.hpp"
#include "Pomdog/Signals/Connection.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace Pomdog::UI {

class UIEventDispatcher : public std::enable_shared_from_this<UIEventDispatcher> {
public:
    UIEventDispatcher(
        const std::shared_ptr<GameWindow>& window,
        const std::shared_ptr<Keyboard>& keyboard);

    void UpdateChildren();

    void Touch(const MouseState& mouseState);

    void UpdateAnimation(const Duration& frameDuration);

    Connection Connect(const std::weak_ptr<UIElement>& child);

    void RemoveChild(const std::weak_ptr<UIElement>& child);

    bool Contains(const std::weak_ptr<UIElement>& child) const;

private:
    void AddChild(std::weak_ptr<UIElement>&& child);

    void PointerEntered(
        const Point2D& position,
        const MouseState& mouseState,
        const std::shared_ptr<UIElement>& node);

    void PointerExited(const Point2D& position);

    void PointerPressed(const Point2D& position);

    void PointerMoved(const Point2D& position);

    void PointerReleased(const Point2D& position);

    std::optional<PointerMouseEvent> FindPointerMouseEvent(const MouseState& mouseState) const;

    ButtonState CheckMouseButton(
        const MouseState& mouseState,
        const PointerMouseEvent& pointerMouseEvent) const;

private:
    struct PointerState final {
        std::weak_ptr<UIElement> focusedElement;
        PointerPoint pointerPoint;
        std::int32_t PrevScrollWheel;
        Duration lastClickTime;
        Point2D lastClickPosition;
    };

    std::vector<std::weak_ptr<UIElement>> subscribeRequests;
    std::vector<std::weak_ptr<UIElement>> children;
    std::unique_ptr<PointerState> pointerState;
    std::shared_ptr<GameWindow> window;
    std::shared_ptr<Keyboard> keyboard;
    std::weak_ptr<UIElement> focusedElement;
    ScopedConnection keyDownConn;
    ScopedConnection keyUpConn;
    ScopedConnection textInputConn;
};

} // namespace Pomdog::UI
