// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_window.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/pointer_event_type.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/widget.h"
#include "pomdog/input/keyboard.h"
#include "pomdog/input/mouse_state.h"
#include "pomdog/signals/connection.h"
#include "pomdog/signals/scoped_connection.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {

class UIEventDispatcher : public std::enable_shared_from_this<UIEventDispatcher> {
public:
    UIEventDispatcher(
        const std::shared_ptr<GameWindow>& window,
        const std::shared_ptr<Keyboard>& keyboard);

    void Touch(const MouseState& mouseState, std::vector<std::shared_ptr<Widget>>& children);

    void UpdateAnimation(const Duration& frameDuration);

    std::shared_ptr<Widget> GetFocusWidget() const;

    void SetFocusWidget(const std::shared_ptr<Widget>& widget);

    void ClearFocus(const std::shared_ptr<Widget>& widget);

    std::function<void(const std::shared_ptr<Widget>& widget)> AddContextMenu;

    std::function<void(const std::shared_ptr<Widget>& widget)> RemoveContextMenu;

    Signal<void(const std::shared_ptr<Widget>& widget)> FocusChanged;

private:
    void PointerEntered(
        const Point2D& position,
        const MouseState& mouseState,
        const std::shared_ptr<Widget>& node);

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
        std::weak_ptr<Widget> focusedWidget;
        PointerPoint pointerPoint;
        std::int32_t PrevScrollWheel;
        Duration lastClickTime;
        Point2D lastClickPosition;
    };

    std::unique_ptr<PointerState> pointerState;
    std::shared_ptr<GameWindow> window;
    std::shared_ptr<Keyboard> keyboard;
    std::weak_ptr<Widget> focusedWidget;
    ScopedConnection keyDownConn;
    ScopedConnection keyUpConn;
    ScopedConnection textInputConn;
};

} // namespace pomdog::gui
