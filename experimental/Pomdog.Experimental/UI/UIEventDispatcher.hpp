// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/UI/detail/SubscribeRequestDispatcher.hpp"
#include "Pomdog.Experimental/UI/UIElement.hpp"
#include "Pomdog.Experimental/UI/PointerPoint.hpp"
#include "Pomdog.Experimental/UI/PointerEventType.hpp"
#include <Pomdog/Pomdog.hpp>
#include <cstdint>
#include <memory>
#include <vector>

namespace Pomdog {
namespace UI {
namespace Detail {

class UIEventConnection;

} // namespace Detail

class UIEventDispatcher : public std::enable_shared_from_this<UIEventDispatcher> {
public:
    explicit UIEventDispatcher(const std::shared_ptr<GameWindow>& window);

    void UpdateChildren();

    void Touch(const MouseState& mouseState);

    void UpdateAnimation(const Duration& frameDuration);

    Detail::UIEventConnection Connect(const std::weak_ptr<UIElement>& child);

private:
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
