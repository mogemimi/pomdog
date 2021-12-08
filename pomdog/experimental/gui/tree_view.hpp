// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/experimental/gui/disclosure_triangle_button.hpp"
#include "pomdog/experimental/gui/horizontal_layout.hpp"
#include "pomdog/experimental/gui/text_block.hpp"
#include "pomdog/experimental/gui/vertical_layout.hpp"
#include "pomdog/experimental/gui/widget.hpp"
#include "pomdog/signals/scoped_connection.hpp"

namespace pomdog::gui {

class TreeView final
    : public Widget
    , public std::enable_shared_from_this<TreeView> {
public:
    explicit TreeView(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void OnEnter() override;

    void SetText(const std::string& text);

    void SetExpanded(bool expanded);

    void SetVerticalLayoutSpacing(int spacing);

    void SetPosition(const Point2D& position) override;

    void MarkParentTransformDirty() override;

    void MarkContentLayoutDirty() override;

    void Draw(DrawingContext& drawingContext) override;

    void AddChild(const std::shared_ptr<Widget>& widget);

    std::shared_ptr<Widget> GetChildAt(const Point2D& position) override;

    void UpdateAnimation(const Duration& frameDuration) override;

    void DoLayout() override;

private:
    void UpdateLayout();

private:
    std::shared_ptr<HorizontalLayout> horizontalLayout;
    std::shared_ptr<DisclosureTriangleButton> disclosureTriangleButton;
    std::shared_ptr<TextBlock> textBlock;
    std::shared_ptr<VerticalLayout> verticalLayout;
    ScopedConnection connection;
    int layoutSpacing;
    int indentation;
    bool expanded;
    bool needToUpdateLayout;
};

} // namespace pomdog::gui
