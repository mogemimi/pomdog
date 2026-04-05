// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/experimental/gui/disclosure_triangle_button.h"
#include "pomdog/experimental/gui/horizontal_layout.h"
#include "pomdog/experimental/gui/text_block.h"
#include "pomdog/experimental/gui/vertical_layout.h"
#include "pomdog/experimental/gui/widget.h"
#include "pomdog/signals/scoped_connection.h"

namespace pomdog::gui {

class TreeView final
    : public Widget
    , public std::enable_shared_from_this<TreeView> {
public:
    explicit TreeView(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    [[nodiscard]] HorizontalAlignment
    getHorizontalAlignment() const noexcept override;

    [[nodiscard]] VerticalAlignment
    getVerticalAlignment() const noexcept override;

    void
    onEnter() override;

    void
    setText(const std::string& text);

    void
    setExpanded(bool expanded);

    void
    setVerticalLayoutSpacing(int spacing);

    void
    setPosition(const Point2D& position) override;

    void
    markParentTransformDirty() override;

    void
    markContentLayoutDirty() override;

    void
    draw(DrawingContext& drawingContext) override;

    void
    addChild(const std::shared_ptr<Widget>& widget);

    [[nodiscard]] std::shared_ptr<Widget>
    getChildAt(const Point2D& position) override;

    void
    updateAnimation(const Duration& frameDuration) override;

    void
    doLayout() override;

private:
    void updateLayout();

private:
    std::shared_ptr<HorizontalLayout> horizontalLayout_;
    std::shared_ptr<DisclosureTriangleButton> disclosureTriangleButton_;
    std::shared_ptr<TextBlock> textBlock_;
    std::shared_ptr<VerticalLayout> verticalLayout_;
    ScopedConnection connection_;
    int layoutSpacing_;
    int indentation_;
    bool expanded_;
    bool needToUpdateLayout_;
};

} // namespace pomdog::gui
