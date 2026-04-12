// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/mouse_cursor.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/experimental/gui/hierarchy_sort_order.h"
#include "pomdog/experimental/gui/horizontal_alignment.h"
#include "pomdog/experimental/gui/vertical_alignment.h"
#include "pomdog/math/point2d.h"
#include "pomdog/math/rect2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
enum class Keys : u8;
class Keyboard;
} // namespace pomdog

namespace pomdog::gui {
class DrawingContext;
class UIEventDispatcher;
class PointerPoint;
} // namespace pomdog::gui

namespace pomdog::gui {

class Widget {
public:
    explicit Widget(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    virtual ~Widget();

    [[nodiscard]] std::shared_ptr<UIEventDispatcher>
    getDispatcher() const;

    void
    setParent(const std::shared_ptr<Widget>& parentIn);

    [[nodiscard]] std::shared_ptr<Widget const>
    getParent() const;

    [[nodiscard]] std::shared_ptr<Widget>
    getParent();

    void
    setSize(int width, int height);

    [[nodiscard]] Rect2D
    getBounds() const noexcept;

    [[nodiscard]] int
    getX() const noexcept;

    [[nodiscard]] int
    getY() const noexcept;

    [[nodiscard]] int
    getWidth() const noexcept;

    [[nodiscard]] int
    getHeight() const noexcept;

    [[nodiscard]] bool
    isVisible() const noexcept;

    void
    setVisible(bool visible) noexcept;

    [[nodiscard]] bool
    isInteractable() const noexcept;

    void
    setInteractable(bool interactable) noexcept;

    [[nodiscard]] bool
    isWheelFocusEnabled() const noexcept;

    void
    setWheelFocusEnabled(bool wheelFocusEnabled) noexcept;

    [[nodiscard]] HierarchySortOrder
    getHierarchySortOrder() const noexcept;

    void
    setHierarchySortOrder(HierarchySortOrder sortOrder) noexcept;

    [[nodiscard]] Point2D
    getPosition() const noexcept;

    virtual void
    setPosition(const Point2D& position);

    [[nodiscard]] Point2D
    getGlobalPosition();

    virtual void
    markParentTransformDirty();

    virtual void
    markContentLayoutDirty();

    [[nodiscard]] virtual std::shared_ptr<Widget>
    getChildAt(const Point2D& position);

    virtual void
    doLayout();

    [[nodiscard]] virtual bool
    getSizeToFitContent() const noexcept;

    [[nodiscard]] virtual HorizontalAlignment
    getHorizontalAlignment() const noexcept;

    [[nodiscard]] virtual VerticalAlignment
    getVerticalAlignment() const noexcept;

    virtual void
    draw(DrawingContext& drawingContext);

    virtual void
    updateAnimation(const Duration& frameDuration);

    virtual void
    onEnter();

    virtual void
    onFocusIn();

    virtual void
    onFocusOut();

    virtual void
    onTextInput(const Keyboard& keyboard, const std::string& text);

    virtual void
    onKeyDown(const Keyboard& keyboard, Keys key);

    virtual void
    onKeyUp(const Keyboard& keyboard, Keys key);

    virtual void
    onPointerCanceled(const PointerPoint& pointerPoint);

    virtual void
    onPointerCaptureLost(const PointerPoint& pointerPoint);

    virtual void
    onPointerEntered(const PointerPoint& pointerPoint);

    virtual void
    onPointerExited(const PointerPoint& pointerPoint);

    virtual void
    onPointerMoved(const PointerPoint& pointerPoint);

    virtual void
    onPointerPressed(const PointerPoint& pointerPoint);

    virtual void
    onPointerReleased(const PointerPoint& pointerPoint);

    virtual void
    onPointerWheelChanged(const PointerPoint& pointerPoint);

    void
    setCursor(MouseCursor cursor);

    void
    resetCursor();

    [[nodiscard]] std::optional<MouseCursor>
    getCurrentCursor() const;

private:
    Point2D localPosition_;
    Point2D parentPosition_;
    std::weak_ptr<UIEventDispatcher> weakDispatcher_;
    std::weak_ptr<Widget> weakParent_;
    int height_;
    int width_;
    std::optional<MouseCursor> cursor_;
    HierarchySortOrder hierarchySortOrder_;
    bool isParentTransformDirty_;
    bool isVisible_;
    bool isInteractable_;
    bool isWheelFocusEnabled_;
};

} // namespace pomdog::gui
