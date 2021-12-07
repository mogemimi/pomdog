// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/duration.hpp"
#include "pomdog/application/mouse_cursor.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/gui/hierarchy_sort_order.hpp"
#include "pomdog/experimental/gui/horizontal_alignment.hpp"
#include "pomdog/experimental/gui/vertical_alignment.hpp"
#include "pomdog/math/point2d.hpp"
#include "pomdog/math/rectangle.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {
enum class Keys : std::uint8_t;
class KeyboardState;
} // namespace Pomdog

namespace Pomdog::GUI {

class DrawingContext;
class UIEventDispatcher;
class PointerPoint;

class Widget {
public:
    explicit Widget(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    virtual ~Widget();

    std::shared_ptr<UIEventDispatcher> GetDispatcher() const;

    void SetParent(const std::shared_ptr<Widget>& parentIn);
    std::shared_ptr<Widget const> GetParent() const;
    std::shared_ptr<Widget> GetParent();

    void SetSize(int width, int height);

    Rectangle GetBounds() const noexcept;

    int GetX() const noexcept;
    int GetY() const noexcept;
    int GetWidth() const noexcept;
    int GetHeight() const noexcept;

    bool IsVisible() const noexcept;
    void SetVisible(bool visible) noexcept;

    bool IsInteractable() const noexcept;
    void SetInteractable(bool interactable) noexcept;

    bool IsWheelFocusEnabled() const noexcept;
    void SetWheelFocusEnabled(bool wheelFocusEnabled) noexcept;

    HierarchySortOrder GetHierarchySortOrder() const noexcept;
    void SetHierarchySortOrder(HierarchySortOrder sortOrder) noexcept;

    Point2D GetPosition() const noexcept;
    virtual void SetPosition(const Point2D& position);

    Point2D GetGlobalPosition();

    virtual void MarkParentTransformDirty();

    virtual void MarkContentLayoutDirty();

    virtual std::shared_ptr<Widget> GetChildAt(const Point2D& position);

    virtual void DoLayout();

    virtual bool GetSizeToFitContent() const noexcept;
    virtual HorizontalAlignment GetHorizontalAlignment() const noexcept;
    virtual VerticalAlignment GetVerticalAlignment() const noexcept;

    virtual void Draw(DrawingContext& drawingContext);

    virtual void UpdateAnimation(const Duration& frameDuration);

    virtual void OnEnter();

    virtual void OnFocusIn();

    virtual void OnFocusOut();

    virtual void OnTextInput(const KeyboardState& keyboardState, const std::string& text);

    virtual void OnKeyDown(const KeyboardState& keyboardState, Keys key);

    virtual void OnKeyUp(const KeyboardState& keyboardState, Keys key);

    virtual void OnPointerCanceled(const PointerPoint& pointerPoint);

    virtual void OnPointerCaptureLost(const PointerPoint& pointerPoint);

    virtual void OnPointerEntered(const PointerPoint& pointerPoint);

    virtual void OnPointerExited(const PointerPoint& pointerPoint);

    virtual void OnPointerMoved(const PointerPoint& pointerPoint);

    virtual void OnPointerPressed(const PointerPoint& pointerPoint);

    virtual void OnPointerReleased(const PointerPoint& pointerPoint);

    virtual void OnPointerWheelChanged(const PointerPoint& pointerPoint);

    void SetCursor(MouseCursor cursor);

    void ResetCursor();

    std::optional<MouseCursor> GetCurrentCursor() const;

private:
    Point2D localPosition;
    Point2D parentPosition;
    std::weak_ptr<UIEventDispatcher> weakDispatcher;
    std::weak_ptr<Widget> weakParent;
    int height;
    int width;
    std::optional<MouseCursor> cursor;
    HierarchySortOrder hierarchySortOrder;
    bool isParentTransformDirty;
    bool isVisible;
    bool isInteractable;
    bool isWheelFocusEnabled;
};

} // namespace Pomdog::GUI
