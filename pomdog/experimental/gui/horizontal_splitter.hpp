// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/gui/thickness.hpp"
#include "pomdog/experimental/gui/widget.hpp"
#include "pomdog/math/color.hpp"
#include "pomdog/signals/scoped_connection.hpp"
#include "pomdog/signals/signal.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::GUI {

class HorizontalSplitterHandle;

class HorizontalSplitter final
    : public Widget
    , public std::enable_shared_from_this<HorizontalSplitter> {
public:
    HorizontalSplitter(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        int widthIn,
        int heightIn);

    void SetMargin(const Thickness& margin);

    void SetLayoutSpacing(int spacing);

    void SetBackgroundColor(const Color& color);

    void SetHandleColor(const Color& color);

    void SetPosition(const Point2D& position) override;

    void MarkParentTransformDirty() override;

    void MarkContentLayoutDirty() override;

    bool GetSizeToFitContent() const noexcept override;

    void OnEnter() override;

    void Draw(DrawingContext& drawingContext) override;

    void AddChild(const std::shared_ptr<Widget>& widget);

    std::shared_ptr<Widget> GetChildAt(const Point2D& position) override;

    void UpdateAnimation(const Duration& frameDuration) override;

    void SetMinimumWidth(int index, int minimumWidth);

    void SetMinimumWidth(const std::shared_ptr<Widget>& widget, int minimumWidth);

    void DoLayout() override;

private:
    void UpdateLayout();

private:
    struct Item final {
        std::shared_ptr<Widget> widget;
        int minimumWidth = 0;
    };

    std::shared_ptr<HorizontalSplitterHandle> splitterHandle;
    std::shared_ptr<HorizontalSplitter> childSplitter;
    Connection resizingConn;
    std::array<Item, 2> children;
    Thickness margin;
    Color backgroundColor;
    bool needToUpdateLayout;
};

} // namespace Pomdog::GUI
