// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/gui/thickness.hpp"
#include "pomdog/experimental/gui/vertical_layout.hpp"
#include "pomdog/experimental/gui/widget.hpp"
#include "pomdog/math/vector2.hpp"
#include "pomdog/signals/scoped_connection.hpp"
#include "pomdog/signals/signal.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::GUI {

class ListView;

class ListViewItem final
    : public Widget
    , public std::enable_shared_from_this<ListViewItem> {
public:
    explicit ListViewItem(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    bool IsEnabled() const noexcept;
    void SetEnabled(bool isEnabled);

    bool IsHovered() const noexcept;
    bool IsFocused() const noexcept;

    bool IsSelected() const noexcept;
    void SetSelected(bool selected);

    int GetListViewIndex() const noexcept;
    void SetListViewIndex(int index) noexcept;

    void SetWidget(const std::shared_ptr<Widget>& widget);

    void SetMargin(const Thickness& margin);

    void SetHorizontalAlignment(HorizontalAlignment horizontalAlignment) noexcept;

    void SetVerticalAlignment(VerticalAlignment verticalAlignment) noexcept;

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void OnEnter() override;

    void OnFocusIn() override;

    void OnFocusOut() override;

    void OnPointerEntered(const PointerPoint& pointerPoint) override;

    void OnPointerExited(const PointerPoint& pointerPoint) override;

    void OnPointerPressed(const PointerPoint& pointerPoint) override;

    void OnPointerReleased(const PointerPoint& pointerPoint) override;

    void Draw(DrawingContext& drawingContext) override;

    std::shared_ptr<Widget> GetChildAt(const Point2D& position) override;

    std::shared_ptr<Widget> GetChild();

    void UpdateAnimation(const Duration& frameDuration) override;

    void DoLayout() override;

private:
    std::shared_ptr<Widget> child;
    std::int32_t listViewIndex;
    std::int16_t marginLeft;
    std::int16_t marginRight;
    HorizontalAlignment horizontalAlignment;
    VerticalAlignment verticalAlignment;
    bool isEnabled;
    bool isHovered;
    bool isFocused;
    bool isSelected;
};

} // namespace Pomdog::GUI
