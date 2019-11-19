// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/UI/Thickness.hpp"
#include "Pomdog/Experimental/UI/UIElement.hpp"
#include "Pomdog/Experimental/UI/VerticalLayout.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include <list>
#include <memory>
#include <optional>

namespace Pomdog::UI {

class StackPanel final
    : public UIElement
    , public std::enable_shared_from_this<StackPanel> {
public:
    StackPanel(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        int widthIn,
        int heightIn);

    void SetTransform(const Matrix3x2& matrix) override;

    void MarkParentTransformDirty() override;

    void MarkContentLayoutDirty() override;

    bool SizeToFitContent() const override;

    void OnEnter() override;

    void OnPointerPressed(const PointerPoint& pointerPoint) override;

    void OnPointerMoved(const PointerPoint& pointerPoint) override;

    void OnPointerReleased(const PointerPoint& pointerPoint) override;

    void Draw(DrawingContext& drawingContext) override;

    void UpdateAnimation(const Duration& frameDuration) override;

    void AddChild(const std::shared_ptr<UIElement>& element);

    void DoLayout() override;

    void SetPadding(const Thickness& padding);

private:
    void UpdateLayout();

private:
    std::shared_ptr<VerticalLayout> verticalLayout;
    ScopedConnection connection;
    Thickness padding;
    int barHeight;
    std::optional<Vector2> startTouchPoint;
    bool needToUpdateLayout;
};

} // namespace Pomdog::UI
