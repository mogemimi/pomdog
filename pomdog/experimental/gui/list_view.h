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
} // namespace pomdog::gui

namespace pomdog::gui {

class ListView final
    : public Widget
    , public std::enable_shared_from_this<ListView> {
public:
    ListView(
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
    onEnter() override;

    void
    draw(DrawingContext& drawingContext) override;

    void
    addChild(const std::shared_ptr<Widget>& widget);

    void
    removeChild(const std::shared_ptr<Widget>& widget);

    void
    removeChild(int index);

    [[nodiscard]] int
    getItemCount() const noexcept;

    [[nodiscard]] std::optional<int>
    getCurrentIndex() const noexcept;

    void
    setCurrentIndex(int index);

    [[nodiscard]] std::shared_ptr<Widget>
    getChildAt(const Point2D& position) override;

    [[nodiscard]] std::shared_ptr<Widget>
    getChildAt(int index);

    void
    updateAnimation(const Duration& frameDuration) override;

    void
    doLayout() override;

    void
    setPadding(const Thickness& padding);

    Signal<void(std::optional<int> index)> CurrentIndexChanged;

private:
    void
    updateLayout();

private:
    std::shared_ptr<VerticalLayout> verticalLayout_;
    Thickness padding_;
    std::optional<int> currentIndex_;
    bool needToUpdateLayout_;
};

} // namespace pomdog::gui
