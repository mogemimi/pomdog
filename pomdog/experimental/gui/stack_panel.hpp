// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/gui/thickness.hpp"
#include "pomdog/experimental/gui/vertical_layout.hpp"
#include "pomdog/experimental/gui/widget.hpp"
#include "pomdog/math/vector2.hpp"
#include "pomdog/signals/scoped_connection.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <list>
#include <memory>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::GUI {

class StackPanel final
    : public Widget
    , public std::enable_shared_from_this<StackPanel> {
public:
    StackPanel(
        const std::shared_ptr<UIEventDispatcher>& dispatcher,
        int widthIn,
        int heightIn);

    void SetPosition(const Point2D& position) override;

    void MarkParentTransformDirty() override;

    void MarkContentLayoutDirty() override;

    bool GetSizeToFitContent() const noexcept override;

    void OnEnter() override;

    void OnPointerPressed(const PointerPoint& pointerPoint) override;

    void OnPointerMoved(const PointerPoint& pointerPoint) override;

    void OnPointerReleased(const PointerPoint& pointerPoint) override;

    void Draw(DrawingContext& drawingContext) override;

    void AddChild(const std::shared_ptr<Widget>& widget);

    std::shared_ptr<Widget> GetChildAt(const Point2D& position) override;

    void UpdateAnimation(const Duration& frameDuration) override;

    void DoLayout() override;

    void SetPadding(const Thickness& padding);

private:
    void UpdateLayout();

private:
    std::shared_ptr<VerticalLayout> verticalLayout;
    ScopedConnection connection;
    Thickness padding;
    int barHeight;
    std::optional<Vector2> startTouchPoint;
    bool needToUpdateLayout;
};

} // namespace Pomdog::GUI
