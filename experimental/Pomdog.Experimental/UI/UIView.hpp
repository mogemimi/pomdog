// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "UIElement.hpp"
#include "UIEventListener.hpp"
#include "Pomdog/Application/MouseCursor.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {
namespace UI {

//using UIElementCollection = std::list<std::shared_ptr<UIElement>>;

class UIView: public UIElement, public UIEventListener {
public:
    UIView(Matrix3x2 const& transform, std::uint32_t width, std::uint32_t height);

    virtual ~UIView() = default;

    Matrix3x2 Transform() const override final;
    void Transform(Matrix3x2 const& matrix) override final;
    void Transform(Matrix3x2 && matrix) override final;
    //Vector2 Origin() const override final { return origin; }

    std::weak_ptr<UIElement const> Parent() const override final;
    std::weak_ptr<UIElement> Parent() override final;

    void Parent(std::weak_ptr<UIElement> const& parentIn);

    virtual void UpdateTransform() override;
    void MarkParentTransformDirty() override final;
    Matrix3x2 GlobalTransform() const override final;

    void MarkParentDrawOrderDirty() override final;
    std::int32_t GlobalDrawOrder() override final;
    void DrawOrder(std::int32_t drawOrder) override final;
    std::int32_t DrawOrder() const override final;

    std::uint16_t Width() const override final { return width; }
    std::uint16_t Height() const override final { return height; }

    void Width(std::uint16_t widthIn) { this->width = widthIn; }
    void Height(std::uint16_t heightIn) { this->height = heightIn; }

    Rectangle BoundingBox() const override;

    UI::HorizontalAlignment HorizontalAlignment() const override { return UI::HorizontalAlignment::Stretch; }
    UI::VerticalAlignment VerticalAlignment() const override { return UI::VerticalAlignment::Stretch; }

    bool SizeToFitContent() const override;

    std::weak_ptr<UIEventDispatcher> Dispatcher() const override;

    void OnParentChanged() override;

    void OnPointerCanceled(PointerPoint const&) override;

    void OnPointerCaptureLost(PointerPoint const&) override;

    void OnPointerWheelChanged(PointerPoint const&) override;

    void OnPointerEntered(PointerPoint const&) override;

    void OnPointerExited(PointerPoint const&) override;

    void OnPointerPressed(PointerPoint const&) override;

    void OnPointerMoved(PointerPoint const&) override;

    void OnPointerReleased(PointerPoint const&) override;

    void OnRenderSizeChanged(std::uint32_t, std::uint32_t) override;

    void Draw(DrawingContext &) override;

    void UpdateAnimation(Duration const&) override;

    void SetCursor(MouseCursor cursor);

    void ResetCursor();

    Optional<MouseCursor> CurrentCursor() const;

private:
    Matrix3x2 transform;
    //Vector2 origin;
    Matrix3x2 parentTransform;
    std::weak_ptr<UIElement> parent;
    Optional<MouseCursor> cursor;
    std::uint32_t parentDrawOrder;
    std::uint32_t drawOrder;
    std::uint16_t height;
    std::uint16_t width;
    bool isParentTransformDirty;
    bool isParentDrawOrderDirty;
};

}// namespace UI
}// namespace Pomdog
