// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/context_menu.h"
#include "pomdog/experimental/gui/font_weight.h"
#include "pomdog/experimental/gui/text_alignment.h"
#include "pomdog/experimental/gui/widget.h"
#include "pomdog/signals/connection_list.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/signals/signal.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {

class PopupMenu final
    : public Widget
    , public std::enable_shared_from_this<PopupMenu> {
public:
    explicit PopupMenu(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);

    bool IsHovered() const;
    bool IsFocused() const;

    void AddItem(const std::string& text);

    void ClearItems();

    std::string GetItem(int index) const;

    int GetItemCount() const noexcept;

    int GetCurrentIndex() const noexcept;

    void SetCurrentIndex(int index);

    void SetFontWeight(FontWeight fontWeight);

    std::string GetText() const;

    void SetTextAlignment(TextAlignment textAlign);

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

    Signal<void(int index)> CurrentIndexChanged;

private:
    struct PopupMenuItem final {
        std::string text;
    };
    std::vector<PopupMenuItem> items;
    int currentIndex = 0;

    ConnectionList connections;
    ScopedConnection focusConn;
    std::shared_ptr<ContextMenu> contextMenu;
    FontWeight fontWeight;
    TextAlignment textAlignment;
    HorizontalAlignment horizontalAlignment;
    VerticalAlignment verticalAlignment;
    bool isEnabled;
    bool isHovered;
    bool isPressed;
    bool isFocused;
};

} // namespace pomdog::gui
