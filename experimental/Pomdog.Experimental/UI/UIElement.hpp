// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "HorizontalAlignment.hpp"
#include "VerticalAlignment.hpp"
#include "Pomdog/Application/MouseCursor.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <Pomdog/Pomdog.hpp>
#include <cstdint>
#include <memory>

namespace Pomdog {
namespace UI {

class DrawingContext;
class UIEventDispatcher;
class PointerPoint;

class UIElement {
public:
    explicit UIElement(std::shared_ptr<UIEventDispatcher> const& dispatcher);

    virtual ~UIElement() = default;

    std::shared_ptr<UIEventDispatcher> Dispatcher() const;

    void Parent(std::shared_ptr<UIElement> const& parentIn);
    std::shared_ptr<UIElement const> Parent() const;
    std::shared_ptr<UIElement> Parent();

    void SetSize(int width, int height);

    Rectangle Bounds() const;

    int Width() const;
    int Height() const;

    void MarkParentDrawOrderDirty();

    int GlobalDrawOrder();

    void DrawOrder(int drawOrder);
    int DrawOrder() const;

    Matrix3x2 Transform() const;
    void Transform(Matrix3x2 const& matrix);
    void Transform(Matrix3x2 && matrix);

    Matrix3x2 GlobalTransform() const;

    void MarkParentTransformDirty();

    virtual void UpdateTransform();

    virtual bool SizeToFitContent() const { return false; }
    virtual HorizontalAlignment HorizontalAlignment() const { return UI::HorizontalAlignment::Stretch; }
    virtual VerticalAlignment VerticalAlignment() const { return UI::VerticalAlignment::Stretch; }

    virtual void Draw(DrawingContext & drawingContext);

    virtual void UpdateAnimation(Duration const& frameDuration);

    virtual void OnEnter();

    virtual void OnRenderSizeChanged(int width, int height);

    virtual void OnPointerCanceled(PointerPoint const& pointerPoint);

    virtual void OnPointerCaptureLost(PointerPoint const& pointerPoint);

    virtual void OnPointerEntered(PointerPoint const& pointerPoint);

    virtual void OnPointerExited(PointerPoint const& pointerPoint);

    virtual void OnPointerMoved(PointerPoint const& pointerPoint);

    virtual void OnPointerPressed(PointerPoint const& pointerPoint);

    virtual void OnPointerReleased(PointerPoint const& pointerPoint);

    virtual void OnPointerWheelChanged(PointerPoint const& pointerPoint);

    void SetCursor(MouseCursor cursor);

    void ResetCursor();

    Optional<MouseCursor> CurrentCursor() const;

//    void Hide();
//    void Show();
//    virtual Thickness Padding() const = 0;
//    virtual Vector2 Origin() const = 0;
//    virtual Thickness Margin() const = 0;

private:
    Matrix3x2 transform;
    //Vector2 origin;
    Matrix3x2 parentTransform;
    std::weak_ptr<UIEventDispatcher> weakDispatcher;
    std::weak_ptr<UIElement> weakParent;
    std::int32_t parentDrawOrder;
    std::int32_t localDrawOrder;
    std::int16_t height;
    std::int16_t width;
    Optional<MouseCursor> cursor;
    bool isParentDrawOrderDirty;
    bool isParentTransformDirty;
};

} // namespace UI
} // namespace Pomdog
