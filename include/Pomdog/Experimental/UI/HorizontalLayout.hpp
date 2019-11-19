// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/UI/Thickness.hpp"
#include "Pomdog/Experimental/UI/UIElement.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include <memory>
#include <vector>

namespace Pomdog::UI {

class HorizontalLayout final
    : public UIElement
    , public std::enable_shared_from_this<HorizontalLayout> {
public:
    HorizontalLayout(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        int widthIn,
        int heightIn);

    void SetMargin(const Thickness& margin);

    void SetLayoutSpacing(int spacing);

    void SetStackedLayout(bool isStackedLayout);

    void SetTransform(const Matrix3x2& matrix) override;

    void MarkParentTransformDirty() override;

    void MarkContentLayoutDirty() override;

    bool SizeToFitContent() const override;

    void OnEnter() override;

    void Draw(DrawingContext& drawingContext) override;

    void AddChild(const std::shared_ptr<UIElement>& element);

    void SetStretchFactor(int index, int stretch);

    void SetStretchFactor(const std::shared_ptr<UIElement>& element, int stretch);

    void DoLayout() override;

private:
    void UpdateLayout();

private:
    struct Item final {
        std::shared_ptr<UIElement> element;
        int stretch = 0;
    };

    std::vector<Item> children;
    ScopedConnection connection;
    Thickness margin;
    int layoutSpacing;
    bool needToUpdateLayout;
    bool isStackedLayout;
};

} // namespace Pomdog::UI
