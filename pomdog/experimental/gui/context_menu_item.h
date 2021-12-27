// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/font_weight.h"
#include "pomdog/experimental/gui/text_alignment.h"
#include "pomdog/experimental/gui/thickness.h"
#include "pomdog/experimental/gui/vertical_layout.h"
#include "pomdog/experimental/gui/widget.h"
#include "pomdog/math/vector2.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/signals/signal.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {

class ContextMenuButton final
    : public Widget
    , public std::enable_shared_from_this<ContextMenuButton> {
public:
    explicit ContextMenuButton(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);

    bool IsHovered() const;
    bool IsFocused() const;

    void SetFontWeight(FontWeight fontWeight);

    std::string GetText() const;
    void SetText(const std::string& text);

    void SetTextAlignment(TextAlignment textAlign);

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

    void UpdateAnimation(const Duration& frameDuration) override;

    void Draw(DrawingContext& drawingContext) override;

    Signal<void()> Click;

    Signal<void()> FocusOut;

private:
    ScopedConnection connection;
    std::string text;
    std::int16_t marginLeft;
    std::int16_t marginRight;
    std::int8_t clickInterval;
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
