// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/GUI/Thickness.hpp"
#include "Pomdog/Experimental/GUI/Widget.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include <list>
#include <memory>

namespace Pomdog::GUI {

class VerticalScrollBar;

class ScrollView final
    : public Widget
    , public std::enable_shared_from_this<ScrollView> {
public:
    ScrollView(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        int widthIn,
        int heightIn);

    void SetMargin(const Thickness& margin);

    void SetPosition(const Point2D& position) override;

    void MarkParentTransformDirty() override;

    void MarkContentLayoutDirty() override;

    bool GetSizeToFitContent() const noexcept override;

    void ScrollToTop();

    void ScrollToEnd();

    void SetHorizontalAlignment(HorizontalAlignment horizontalAlignment) noexcept;

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;

    void OnEnter() override;

    void OnPointerWheelChanged(const PointerPoint& pointerPoint) override;

    void Draw(DrawingContext& drawingContext) override;

    void SetWidget(const std::shared_ptr<Widget>& widget);

    std::shared_ptr<Widget> GetChildAt(const Point2D& position) override;

    void UpdateAnimation(const Duration& frameDuration) override;

    void DoLayout() override;

private:
    void UpdateLayout();

private:
    std::shared_ptr<Widget> child;
    std::shared_ptr<VerticalScrollBar> scrollBar;
    ScopedConnection connection;
    Thickness margin;
    HorizontalAlignment horizontalAlignment;
    bool needToUpdateLayout;
};

} // namespace Pomdog::GUI
