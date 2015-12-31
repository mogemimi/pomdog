// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "detail/SubscribeRequestDispatcher.hpp"
#include "UIElement.hpp"
#include "PointerPoint.hpp"
#include "PointerEventType.hpp"
#include <Pomdog/Pomdog.hpp>
#include <cstdint>
#include <memory>
#include <vector>

namespace Pomdog {
namespace UI {
namespace Detail {

class UIEventConnection;

} // namespace Detail

class UIEventDispatcher: public std::enable_shared_from_this<UIEventDispatcher> {
public:
    explicit UIEventDispatcher(std::shared_ptr<GameWindow> const& window);

    void UpdateChildren();

    void Touch(MouseState const& mouseState);

    void UpdateAnimation(Duration const& frameDuration);

    Detail::UIEventConnection Connect(std::weak_ptr<UIElement> const& child);

private:
    void PointerEntered(Point2D const& position, MouseState const& mouseState,
        std::shared_ptr<UIElement> const& node);

    void PointerExited(Point2D const& position);

    void PointerPressed(Point2D const& position);

    void PointerMoved(Point2D const& position);

    void PointerReleased(Point2D const& position);

    Optional<PointerMouseEvent> FindPointerMouseEvent(MouseState const& mouseState) const;

    ButtonState CheckMouseButton(MouseState const& mouseState, PointerMouseEvent const& pointerMouseEvent) const;

    void Sort();

private:
    struct PointerState {
        std::weak_ptr<UIElement> focusedElement;
        PointerPoint pointerPoint;
        std::int32_t PrevScrollWheel;
    };

    typedef Detail::SubscribeRequestDispatcher<
        std::weak_ptr<UIElement>> SubscribeRequestDispatcherType;

    SubscribeRequestDispatcherType subscribeRequests;
    std::vector<std::weak_ptr<UIElement>> children;
    std::unique_ptr<PointerState> pointerState;
    std::shared_ptr<GameWindow> window;
};

} // namespace UI
} // namespace Pomdog
