// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/UI/HorizontalAlignment.hpp"
#include "Pomdog.Experimental/UI/VerticalAlignment.hpp"
#include "Pomdog/Application/MouseCursor.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <Pomdog/Pomdog.hpp>
#include <cstdint>
#include <memory>
#include <optional>

namespace Pomdog {
namespace UI {

class DrawingContext;
class UIEventDispatcher;
class PointerPoint;

class UIElement {
public:
    explicit UIElement(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    virtual ~UIElement() = default;

    std::shared_ptr<UIEventDispatcher> GetDispatcher() const;

    void SetParent(const std::shared_ptr<UIElement>& parentIn);
    std::shared_ptr<UIElement const> GetParent() const;
    std::shared_ptr<UIElement> GetParent();

    void SetSize(int width, int height);

    Rectangle GetBounds() const;

    int GetWidth() const;
    int GetHeight() const;

    void MarkParentDrawOrderDirty();

    int GetGlobalDrawOrder();

    void SetDrawOrder(int drawOrder);
    int GetDrawOrder() const;

    Matrix3x2 GetTransform() const;

    virtual void SetTransform(const Matrix3x2& matrix);

    Matrix3x2 GetGlobalTransform();

    virtual void MarkParentTransformDirty();

    virtual void MarkContentLayoutDirty();

    virtual void DoLayout();

    virtual bool SizeToFitContent() const;
    virtual HorizontalAlignment GetHorizontalAlignment() const noexcept;
    virtual VerticalAlignment GetVerticalAlignment() const noexcept;

    virtual void Draw(DrawingContext & drawingContext);

    virtual void UpdateAnimation(const Duration& frameDuration);

    virtual void OnEnter();

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

//    void Hide();
//    void Show();
//    virtual Thickness GetPadding() const = 0;
//    virtual Thickness GetMargin() const = 0;

private:
    Matrix3x2 transform;
    //Vector2 origin;
    Matrix3x2 parentTransform;
    std::weak_ptr<UIEventDispatcher> weakDispatcher;
    std::weak_ptr<UIElement> weakParent;
    std::int32_t parentDrawOrder;
    std::int32_t localDrawOrder;
    int height;
    int width;
    std::optional<MouseCursor> cursor;
    bool isParentDrawOrderDirty;
    bool isParentTransformDirty;
};

} // namespace UI
} // namespace Pomdog
