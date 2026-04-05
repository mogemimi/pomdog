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

    void
    setMargin(const Thickness& margin);

    void
    setLayoutSpacing(int spacing);

    void
    setStackedLayout(bool isStackedLayout);

    void
    setPosition(const Point2D& position) override;

    void
    markParentTransformDirty() override;

    void
    markContentLayoutDirty() override;

    [[nodiscard]] bool
    getSizeToFitContent() const noexcept override;

    void
    onEnter() override;

    void
    draw(DrawingContext& drawingContext) override;

    void
    addChild(const std::shared_ptr<Widget>& widget);

    void
    removeChild(const std::shared_ptr<Widget>& widget);

    void
    removeChild(int index);

    [[nodiscard]] std::shared_ptr<Widget>
    getChildAt(const Point2D& position) override;

    [[nodiscard]] std::shared_ptr<Widget>
    getChildAt(int index);

    [[nodiscard]] int
    getItemCount() const noexcept;

    void
    updateAnimation(const Duration& frameDuration) override;

    void
    doLayout() override;

private:
    void updateLayout();

private:
    std::vector<std::shared_ptr<Widget>> children_;
    Thickness margin_;
    int layoutSpacing_;
    bool needToUpdateLayout_;
    bool isStackedLayout_;
};

} // namespace pomdog::gui
