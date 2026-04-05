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

class HorizontalLayout final
    : public Widget
    , public std::enable_shared_from_this<HorizontalLayout> {
public:
    HorizontalLayout(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        int widthIn,
        int heightIn);

    void setMargin(const Thickness& margin);

    void setLayoutSpacing(int spacing);

    void setStackedLayout(bool isStackedLayout);

    void setPosition(const Point2D& position) override;

    void markParentTransformDirty() override;

    void markContentLayoutDirty() override;

    [[nodiscard]] bool
    getSizeToFitContent() const noexcept override;

    void onEnter() override;

    void draw(DrawingContext& drawingContext) override;

    void addChild(const std::shared_ptr<Widget>& widget);

    [[nodiscard]] std::shared_ptr<Widget>
    getChildAt(const Point2D& position) override;

    void updateAnimation(const Duration& frameDuration) override;

    void setStretchFactor(int index, int stretch);

    void setStretchFactor(const std::shared_ptr<Widget>& widget, int stretch);

    void doLayout() override;

private:
    void updateLayout();

private:
    struct Item final {
        std::shared_ptr<Widget> widget;
        int stretch = 0;
    };

    std::vector<Item> children_;
    Thickness margin_;
    int layoutSpacing_;
    bool needToUpdateLayout_;
    bool isStackedLayout_;
};

} // namespace pomdog::gui
