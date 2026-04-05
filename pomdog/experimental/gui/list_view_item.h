// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/gui/thickness.h"
#include "pomdog/experimental/gui/vertical_layout.h"
#include "pomdog/experimental/gui/widget.h"
#include "pomdog/math/vector2.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/signals/signal.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {

class ListView;

class ListViewItem final
    : public Widget
    , public std::enable_shared_from_this<ListViewItem> {
public:
    explicit ListViewItem(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    [[nodiscard]] bool
    isEnabled() const noexcept;

    void
    setEnabled(bool isEnabled);

    [[nodiscard]] bool
    isHovered() const noexcept;

    [[nodiscard]] bool
    isFocused() const noexcept;

    [[nodiscard]] bool
    isSelected() const noexcept;

    void
    setSelected(bool selected);

    [[nodiscard]] int
    getListViewIndex() const noexcept;

    void
    setListViewIndex(int index) noexcept;

    void
    setWidget(const std::shared_ptr<Widget>& widget);

    void
    setMargin(const Thickness& margin);

    void
    setHorizontalAlignment(HorizontalAlignment horizontalAlignment) noexcept;

    void
    setVerticalAlignment(VerticalAlignment verticalAlignment) noexcept;

    [[nodiscard]] HorizontalAlignment
    getHorizontalAlignment() const noexcept override;

    [[nodiscard]] VerticalAlignment
    getVerticalAlignment() const noexcept override;

    void
    onEnter() override;

    void
    onFocusIn() override;

    void
    onFocusOut() override;

    void
    onPointerEntered(const PointerPoint& pointerPoint) override;

    void
    onPointerExited(const PointerPoint& pointerPoint) override;

    void
    onPointerPressed(const PointerPoint& pointerPoint) override;

    void
    onPointerReleased(const PointerPoint& pointerPoint) override;

    void
    draw(DrawingContext& drawingContext) override;

    [[nodiscard]] std::shared_ptr<Widget>
    getChildAt(const Point2D& position) override;

    [[nodiscard]] std::shared_ptr<Widget>
    getChild();

    void
    updateAnimation(const Duration& frameDuration) override;

    void
    doLayout() override;

private:
    std::shared_ptr<Widget> child_;
    i32 listViewIndex_;
    i16 marginLeft_;
    i16 marginRight_;
    HorizontalAlignment horizontalAlignment_;
    VerticalAlignment verticalAlignment_;
    bool isEnabled_;
    bool isHovered_;
    bool isFocused_;
    bool isSelected_;
};

} // namespace pomdog::gui
