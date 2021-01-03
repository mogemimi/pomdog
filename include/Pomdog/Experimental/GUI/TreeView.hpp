// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/GUI/DisclosureTriangleButton.hpp"
#include "Pomdog/Experimental/GUI/HorizontalLayout.hpp"
#include "Pomdog/Experimental/GUI/TextBlock.hpp"
#include "Pomdog/Experimental/GUI/VerticalLayout.hpp"
#include "Pomdog/Experimental/GUI/Widget.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"

namespace Pomdog::GUI {

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

} // namespace Pomdog::GUI
