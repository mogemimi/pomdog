// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/GUI/Thickness.hpp"
#include "Pomdog/Experimental/GUI/Widget.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::GUI {

class HorizontalLayout final
    : public Widget
    , public std::enable_shared_from_this<HorizontalLayout> {
public:
    HorizontalLayout(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        int widthIn,
        int heightIn);

    void SetMargin(const Thickness& margin);

    void SetLayoutSpacing(int spacing);

    void SetStackedLayout(bool isStackedLayout);

    void SetPosition(const Point2D& position) override;

    void MarkParentTransformDirty() override;

    void MarkContentLayoutDirty() override;

    bool GetSizeToFitContent() const noexcept override;

    void OnEnter() override;

    void Draw(DrawingContext& drawingContext) override;

    void AddChild(const std::shared_ptr<Widget>& widget);

    std::shared_ptr<Widget> GetChildAt(const Point2D& position) override;

    void UpdateAnimation(const Duration& frameDuration) override;

    void SetStretchFactor(int index, int stretch);

    void SetStretchFactor(const std::shared_ptr<Widget>& widget, int stretch);

    void DoLayout() override;

private:
    void UpdateLayout();

private:
    struct Item final {
        std::shared_ptr<Widget> widget;
        int stretch = 0;
    };

    std::vector<Item> children;
    Thickness margin;
    int layoutSpacing;
    bool needToUpdateLayout;
    bool isStackedLayout;
};

} // namespace Pomdog::GUI
