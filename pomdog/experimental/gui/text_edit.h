// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/gui/font_size.h"
#include "pomdog/experimental/gui/font_weight.h"
#include "pomdog/experimental/gui/thickness.h"
#include "pomdog/experimental/gui/widget.h"
#include "pomdog/input/keyboard_state.h"
#include "pomdog/math/color.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/signals/signal.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class SpriteFont;
} // namespace pomdog

namespace pomdog::gui {

class TextEdit final
    : public Widget
    , public std::enable_shared_from_this<TextEdit> {
public:
    explicit TextEdit(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    void
    setFontWeight(FontWeight fontWeight);

    void
    setFontSize(FontSize fontSize);

    [[nodiscard]] bool
    isFocused() const;

    [[nodiscard]] bool
    isEnabled() const;

    void
    setEnabled(bool isEnabled);

    [[nodiscard]] bool
    isReadOnly() const;

    void
    setReadOnly(bool readOnly);

    [[nodiscard]] bool
    isAcceptable() const;

    void
    setAcceptable(bool acceptable);

    [[nodiscard]] std::string
    getText() const;

    void
    setText(const std::string& text);

    [[nodiscard]] std::string
    getPlaceholderText() const;

    void
    setPlaceholderText(const std::string& text);

    [[nodiscard]] Thickness
    getTextMargin() const;

    void
    setTextMargin(const Thickness& margin);

    void
    setBaselineHeight(float pixelSize);

    [[nodiscard]] bool
    hasSelectedText() const;

    void
    setSelection(int start, int length);

    void
    deselect();

    void
    setHorizontalAlignment(HorizontalAlignment horizontalAlignment) noexcept;

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
    onTextInput(const KeyboardState& keyboardState, const std::string& text) override;

    void
    onKeyDown(const KeyboardState& keyboardState, Keys key) override;

    void
    onKeyUp(const KeyboardState& keyboardState, Keys key) override;

    void
    onPointerEntered(const PointerPoint& pointerPoint) override;

    void
    onPointerExited(const PointerPoint& pointerPoint) override;

    void
    onPointerMoved(const PointerPoint& pointerPoint) override;

    void
    onPointerPressed(const PointerPoint& pointerPoint) override;

    void
    onPointerReleased(const PointerPoint& pointerPoint) override;

    void
    draw(DrawingContext& drawingContext) override;

    void
    updateAnimation(const Duration& frameDuration) override;

    Signal<void()> TextChanged;

    Signal<void()> TextSubmitted;

    Signal<void()> FocusOut;

private:
    std::shared_ptr<SpriteFont> spriteFont_;
    ScopedConnection connection_;
    ScopedConnection keyboardConn_;
    std::string text_;
    std::string placeholderText_;
    Thickness textMargin_;
    Duration cursorBlinkInterval_;
    Duration cursorMoveInterval_;
    std::optional<int> startSelectionPosition_;
    std::optional<int> cursorPosition_;
    std::optional<float> textStartPositionX_;
    float baselineHeight_;
    KeyboardState latestKeys_;
    FontWeight fontWeight_;
    FontSize fontSize_;
    HorizontalAlignment horizontalAlignment_;
    bool isEnabled_;
    bool isReadOnly_;
    bool isAcceptable_;
    bool needToSubmit_;
};

} // namespace pomdog::gui
