// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_window.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/gui/pointer_event_type.h"
#include "pomdog/experimental/gui/pointer_point.h"
#include "pomdog/experimental/gui/widget.h"
#include "pomdog/input/button_state.h"
#include "pomdog/input/keyboard.h"
#include "pomdog/input/mouse.h"
#include "pomdog/signals/connection.h"
#include "pomdog/signals/scoped_connection.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {

/// PointerEventInput is the per-frame pointer snapshot consumed by the GUI
/// dispatcher. It unifies mouse buttons / scroll and touch input so that the
/// dispatcher does not depend on a specific input device.
struct PointerEventInput final {
    /// The accumulated vertical scroll value, normalized so that 1.0 equals
    /// one scroll notch.
    f64 scrollY = 0.0;

    /// Whether the primary (left) button is pressed. A pressed touch maps to
    /// this button.
    bool leftDown = false;

    /// Whether the right mouse button is pressed.
    bool rightDown = false;

    /// Whether the middle mouse button is pressed.
    bool middleDown = false;

    /// Whether the first extended mouse button (X1) is pressed.
    bool x1Down = false;

    /// Whether the second extended mouse button (X2) is pressed.
    bool x2Down = false;
};

class UIEventDispatcher : public std::enable_shared_from_this<UIEventDispatcher> {
public:
    UIEventDispatcher(
        const std::shared_ptr<GameWindow>& window,
        const std::shared_ptr<Keyboard>& keyboard);

    void
    touch(const Point2D& position, const PointerEventInput& input, std::vector<std::shared_ptr<Widget>>& children);

    void
    updateAnimation(const Duration& frameDuration);

    [[nodiscard]] std::shared_ptr<Widget>
    getFocusWidget() const;

    void
    setFocusWidget(const std::shared_ptr<Widget>& widget);

    void
    clearFocus(const std::shared_ptr<Widget>& widget);

    void
    dispatchKeyDown(Keys key);

    void
    dispatchKeyUp(Keys key);

    void
    dispatchTextInput(const std::string& text);

    void
    dispatchTextInputFromKeyboard();

    Signal<void(const std::shared_ptr<Widget>& widget)> AddContextMenu;

    Signal<void(const std::shared_ptr<Widget>& widget)> RemoveContextMenu;

    Signal<void(const std::shared_ptr<Widget>& widget)> FocusChanged;

private:
    void pointerEntered(
        const Point2D& position,
        const PointerEventInput& input,
        const std::shared_ptr<Widget>& node);

    void pointerExited(const Point2D& position);

    void pointerPressed(const Point2D& position);

    void pointerMoved(const Point2D& position);

    void pointerReleased(const Point2D& position);

    [[nodiscard]] std::optional<PointerMouseEvent>
    findPointerMouseEvent(const PointerEventInput& input) const;

    [[nodiscard]] ButtonState
    checkMouseButton(
        const PointerEventInput& input,
        const PointerMouseEvent& pointerMouseEvent) const;

private:
    struct PointerState final {
        std::weak_ptr<Widget> focusedWidget_;
        PointerPoint pointerPoint;
        f64 prevScrollWheel;
        Duration lastClickTime;
        Point2D lastClickPosition;
    };

    std::unique_ptr<PointerState> pointerState_;
    std::shared_ptr<GameWindow> window_;
    std::shared_ptr<Keyboard> keyboard_;
    std::weak_ptr<Widget> focusedWidget_;
};

} // namespace pomdog::gui
