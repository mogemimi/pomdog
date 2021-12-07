// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/gui/font_size.hpp"
#include "pomdog/experimental/gui/font_weight.hpp"
#include "pomdog/experimental/gui/thickness.hpp"
#include "pomdog/experimental/gui/widget.hpp"
#include "pomdog/input/keyboard_state.hpp"
#include "pomdog/math/color.hpp"
#include "pomdog/signals/scoped_connection.hpp"
#include "pomdog/signals/signal.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {
class SpriteFont;
} // namespace Pomdog

namespace Pomdog::GUI {

class TextEdit final
    : public Widget
    , public std::enable_shared_from_this<TextEdit> {
public:
    explicit TextEdit(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    void SetFontWeight(FontWeight fontWeight);

    void SetFontSize(FontSize fontSize);

    bool IsFocused() const;

    bool IsEnabled() const;
    void SetEnabled(bool isEnabled);

    bool IsReadOnly() const;
    void SetReadOnly(bool readOnly);

    bool IsAcceptable() const;
    void SetAcceptable(bool acceptable);

    std::string GetText() const;
    void SetText(const std::string& text);

    std::string GetPlaceholderText() const;
    void SetPlaceholderText(const std::string& text);

    Thickness GetTextMargin() const;
    void SetTextMargin(const Thickness& margin);

    void SetBaselineHeight(float pixelSize);

    // void SetMaximumLength(int maxLength);
    // std::string GetSelectedText() const;

    bool HasSelectedText() const;

    void SetSelection(int start, int length);

    void Deselect();

    void SetHorizontalAlignment(HorizontalAlignment horizontalAlignment) noexcept;

    HorizontalAlignment GetHorizontalAlignment() const noexcept override;
    VerticalAlignment GetVerticalAlignment() const noexcept override;

    void OnEnter() override;

    void OnFocusIn() override;

    void OnFocusOut() override;

    void OnTextInput(const KeyboardState& keyboardState, const std::string& text) override;

    void OnKeyDown(const KeyboardState& keyboardState, Keys key) override;

    void OnKeyUp(const KeyboardState& keyboardState, Keys key) override;

    void OnPointerEntered(const PointerPoint& pointerPoint) override;

    void OnPointerExited(const PointerPoint& pointerPoint) override;

    void OnPointerMoved(const PointerPoint& pointerPoint) override;

    void OnPointerPressed(const PointerPoint& pointerPoint) override;

    void OnPointerReleased(const PointerPoint& pointerPoint) override;

    void Draw(DrawingContext& drawingContext) override;

    void UpdateAnimation(const Duration& frameDuration) override;

    Signal<void()> TextChanged;

    Signal<void()> TextSubmitted;

    Signal<void()> FocusOut;

private:
    std::shared_ptr<SpriteFont> spriteFont;
    ScopedConnection connection;
    ScopedConnection keyboardConn;
    std::string text;
    std::string placeholderText;
    Thickness textMargin;
    Duration cursorBlinkInterval;
    Duration cursorMoveInterval;
    std::optional<int> startSelectionPosition;
    std::optional<int> cursorPosition;
    std::optional<float> textStartPositionX;
    float baselineHeight;
    KeyboardState latestKeys;
    FontWeight fontWeight;
    FontSize fontSize;
    HorizontalAlignment horizontalAlignment;
    bool isEnabled;
    bool isReadOnly;
    bool isAcceptable;
    bool needToSubmit;
};

} // namespace Pomdog::GUI
