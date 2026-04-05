// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
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

    [[nodiscard]] bool
    isEnabled() const;

    void
    setEnabled(bool isEnabled);

    [[nodiscard]] bool
    isHovered() const;

    [[nodiscard]] bool
    isFocused() const;

    void
    setFontWeight(FontWeight fontWeight);

    [[nodiscard]] std::string
    getText() const;

    void
    setText(const std::string& text);

    void
    setTextAlignment(TextAlignment textAlign);

    void
    setMargin(const Thickness& margin);

    void
    setHorizontalAlignment(HorizontalAlignment horizontalAlignment) noexcept;

    void
    setVerticalAlignment(VerticalAlignment verticalAlignment) noexcept;

    [[nodiscard]] HorizontalAlignment
    getHorizontalAlignment() const noexcept override;

    [[nodiscard]] VerticalAlignment
    getVerticalAlignment() const noexcept override;

    void
    onEnter() override;

    void
    onFocusIn() override;

    void
    onFocusOut() override;

    void
    onPointerEntered(const PointerPoint& pointerPoint) override;

    void
    onPointerExited(const PointerPoint& pointerPoint) override;

    void
    onPointerPressed(const PointerPoint& pointerPoint) override;

    void
    onPointerReleased(const PointerPoint& pointerPoint) override;

    void
    updateAnimation(const Duration& frameDuration) override;

    void
    draw(DrawingContext& drawingContext) override;

    Signal<void()> Click;

    Signal<void()> FocusOut;

private:
    ScopedConnection connection_;
    std::string text_;
    i16 marginLeft_;
    i16 marginRight_;
    i8 clickInterval_;
    FontWeight fontWeight_;
    TextAlignment textAlignment_;
    HorizontalAlignment horizontalAlignment_;
    VerticalAlignment verticalAlignment_;
    bool isEnabled_;
    bool isHovered_;
    bool isPressed_;
    bool isFocused_;
};

} // namespace pomdog::gui
