// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_window.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/gui/pointer_event_type.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/widget.h"
#include "pomdog/input/keyboard.h"
#include "pomdog/input/mouse_state.h"
#include "pomdog/signals/connection.h"
#include "pomdog/signals/scoped_connection.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {

class UIEventDispatcher : public std::enable_shared_from_this<UIEventDispatcher> {
public:
    UIEventDispatcher(
        const std::shared_ptr<GameWindow>& window,
        const std::shared_ptr<Keyboard>& keyboard);

    void
    touch(const MouseState& mouseState, std::vector<std::shared_ptr<Widget>>& children);

    void
    updateAnimation(const Duration& frameDuration);

    [[nodiscard]] std::shared_ptr<Widget>
    getFocusWidget() const;

    void
    setFocusWidget(const std::shared_ptr<Widget>& widget);

    void
    clearFocus(const std::shared_ptr<Widget>& widget);

    std::function<void(const std::shared_ptr<Widget>& widget)> AddContextMenu;

    std::function<void(const std::shared_ptr<Widget>& widget)> RemoveContextMenu;

    Signal<void(const std::shared_ptr<Widget>& widget)> FocusChanged;

private:
    void pointerEntered(
        const Point2D& position,
        const MouseState& mouseState,
        const std::shared_ptr<Widget>& node);

    void pointerExited(const Point2D& position);

    void pointerPressed(const Point2D& position);

    void pointerMoved(const Point2D& position);

    void pointerReleased(const Point2D& position);

    [[nodiscard]] std::optional<PointerMouseEvent>
    findPointerMouseEvent(const MouseState& mouseState) const;

    [[nodiscard]] ButtonState
    checkMouseButton(
        const MouseState& mouseState,
        const PointerMouseEvent& pointerMouseEvent) const;

private:
    struct PointerState final {
        std::weak_ptr<Widget> focusedWidget_;
        PointerPoint pointerPoint;
        i32 prevScrollWheel;
        Duration lastClickTime;
        Point2D lastClickPosition;
    };

    std::unique_ptr<PointerState> pointerState_;
    std::shared_ptr<GameWindow> window_;
    std::shared_ptr<Keyboard> keyboard_;
    std::weak_ptr<Widget> focusedWidget_;
    ScopedConnection keyDownConn_;
    ScopedConnection keyUpConn_;
    ScopedConnection textInputConn_;
};

} // namespace pomdog::gui
