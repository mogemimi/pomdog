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

    void
    setMargin(const Thickness& margin);

    void
    setBackgroundColor(const Color& color) noexcept;

    void
    setPosition(const Point2D& position) override;

    void
    markParentTransformDirty() override;

    void
    markContentLayoutDirty() override;

    [[nodiscard]] bool
    getSizeToFitContent() const noexcept override;

    void
    scrollToTop();

    void
    scrollToEnd();

    void
    scrollTo(const std::shared_ptr<Widget>& widget);

    void
    setHorizontalAlignment(HorizontalAlignment horizontalAlignment) noexcept;

    [[nodiscard]] HorizontalAlignment
    getHorizontalAlignment() const noexcept override;

    void
    onEnter() override;

    void
    onPointerWheelChanged(const PointerPoint& pointerPoint) override;

    void
    draw(DrawingContext& drawingContext) override;

    void
    setWidget(const std::shared_ptr<Widget>& widget);

    [[nodiscard]] std::shared_ptr<Widget>
    getChildAt(const Point2D& position) override;

    void
    updateAnimation(const Duration& frameDuration) override;

    void
    doLayout() override;

private:
    void
    updateLayout();

private:
    std::shared_ptr<Widget> child_;
    std::shared_ptr<VerticalScrollBar> scrollBar_;
    ScopedConnection connection_;
    Thickness margin_;
    Color backgroundColor_;
    HorizontalAlignment horizontalAlignment_;
    bool needToUpdateLayout_;
};

} // namespace pomdog::gui
