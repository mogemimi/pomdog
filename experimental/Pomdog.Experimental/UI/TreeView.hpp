// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/UI/UIElement.hpp"
#include "Pomdog.Experimental/UI/DisclosureTriangleButton.hpp"
#include "Pomdog.Experimental/UI/HorizontalLayout.hpp"
#include "Pomdog.Experimental/UI/VerticalLayout.hpp"
#include "Pomdog.Experimental/UI/TextBlock.hpp"
#include "Pomdog.Experimental/UI/detail/UIEventConnection.hpp"
#include "Pomdog.Experimental/Rendering/Commands/SpriteBatchCommand.hpp"

namespace Pomdog {
namespace UI {

class TreeView final
    : public UIElement
    , public std::enable_shared_from_this<TreeView> {
public:
    explicit TreeView(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void OnEnter() override;

    void SetText(const std::string& text);

    void SetExpanded(bool expanded);

    void SetVerticalLayoutSpacing(int spacing);

    void SetTransform(const Matrix3x2& matrix) override;

    void MarkParentTransformDirty() override;

    void MarkContentLayoutDirty() override;

    void Draw(DrawingContext & drawingContext) override;

    void AddChild(const std::shared_ptr<UIElement>& element);

    void DoLayout() override;

private:
    void UpdateLayout();

private:
    std::shared_ptr<UI::HorizontalLayout> horizontalLayout;
    std::shared_ptr<UI::DisclosureTriangleButton> disclosureTriangleButton;
    std::shared_ptr<UI::TextBlock> textBlock;
    std::shared_ptr<UI::VerticalLayout> verticalLayout;
    ScopedConnection connection;
    int layoutSpacing;
    int indentation;
    bool expanded;
    bool needToUpdateLayout;
};

} // namespace UI
} // namespace Pomdog
