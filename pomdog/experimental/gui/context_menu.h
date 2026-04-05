// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/thickness.h"
#include "pomdog/experimental/gui/widget.h"
#include "pomdog/math/vector2.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/signals/signal.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {

class ScrollView;
class VerticalLayout;

class ContextMenu final
    : public Widget
    , public std::enable_shared_from_this<ContextMenu> {
public:
    ContextMenu(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        int widthIn,
        int heightIn);

    void
    setPosition(const Point2D& position) override;

    void
    markParentTransformDirty() override;

    void
    markContentLayoutDirty() override;

    [[nodiscard]] bool
    getSizeToFitContent() const noexcept override;

    void
    scrollToTop();

    void
    scrollToEnd();

    void
    scrollTo(const std::shared_ptr<Widget>& widget);

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
    doLayout() override;

    void
    setPadding(const Thickness& padding);

private:
    void updateLayout();

private:
    std::shared_ptr<VerticalLayout> verticalLayout_;
    std::shared_ptr<ScrollView> scrollView_;
    ScopedConnection connection_;
    Thickness padding_;
    bool needToUpdateLayout_;
};

} // namespace pomdog::gui
