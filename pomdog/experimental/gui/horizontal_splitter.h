// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/thickness.h"
#include "pomdog/experimental/gui/widget.h"
#include "pomdog/math/color.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/signals/signal.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {
class HorizontalSplitterHandle;
} // namespace pomdog::gui

namespace pomdog::gui {

class HorizontalSplitter final
    : public Widget
    , public std::enable_shared_from_this<HorizontalSplitter> {
public:
    HorizontalSplitter(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        int widthIn,
        int heightIn);

    void
    setMargin(const Thickness& margin);

    void
    setLayoutSpacing(int spacing);

    void
    setBackgroundColor(const Color& color);

    void
    setHandleColor(const Color& color);

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

    [[nodiscard]] std::shared_ptr<Widget>
    getChildAt(const Point2D& position) override;

    void
    updateAnimation(const Duration& frameDuration) override;

    void
    setMinimumWidth(int index, int minimumWidth);

    void
    setMinimumWidth(const std::shared_ptr<Widget>& widget, int minimumWidth);

    void
    doLayout() override;

private:
    void
    updateLayout();

private:
    struct Item final {
        std::shared_ptr<Widget> widget;
        int minimumWidth = 0;
    };

    std::shared_ptr<HorizontalSplitterHandle> splitterHandle_;
    std::shared_ptr<HorizontalSplitter> childSplitter_;
    Connection resizingConn_;
    std::array<Item, 2> children_;
    Thickness margin_;
    Color backgroundColor_;
    bool needToUpdateLayout_;
};

} // namespace pomdog::gui
