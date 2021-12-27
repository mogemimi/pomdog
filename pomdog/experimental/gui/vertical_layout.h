// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/thickness.h"
#include "pomdog/experimental/gui/widget.h"
#include "pomdog/signals/scoped_connection.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {

class VerticalLayout final
    : public Widget
    , public std::enable_shared_from_this<VerticalLayout> {
public:
    VerticalLayout(
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

    void RemoveChild(const std::shared_ptr<Widget>& widget);

    void RemoveChild(int index);

    std::shared_ptr<Widget> GetChildAt(const Point2D& position) override;

    std::shared_ptr<Widget> GetChildAt(int index);

    int GetItemCount() const noexcept;

    void UpdateAnimation(const Duration& frameDuration) override;

    void DoLayout() override;

private:
    void UpdateLayout();

private:
    std::vector<std::shared_ptr<Widget>> children;
    Thickness margin;
    int layoutSpacing;
    bool needToUpdateLayout;
    bool isStackedLayout;
};

} // namespace pomdog::gui
