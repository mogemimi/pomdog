// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/thickness.h"
#include "pomdog/experimental/gui/widget.h"
#include "pomdog/math/color.h"
#include "pomdog/signals/scoped_connection.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <list>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {

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

    void SetBackgroundColor(const Color& color) noexcept;

    void SetPosition(const Point2D& position) override;

    void MarkParentTransformDirty() override;

    void MarkContentLayoutDirty() override;

    bool GetSizeToFitContent() const noexcept override;

    void ScrollToTop();

    void ScrollToEnd();

    void ScrollTo(const std::shared_ptr<Widget>& widget);

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
    Color backgroundColor;
    HorizontalAlignment horizontalAlignment;
    bool needToUpdateLayout;
};

} // namespace pomdog::gui
