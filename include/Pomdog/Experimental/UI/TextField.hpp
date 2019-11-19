// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/UI/FontSize.hpp"
#include "Pomdog/Experimental/UI/FontWeight.hpp"
#include "Pomdog/Experimental/UI/Thickness.hpp"
#include "Pomdog/Experimental/UI/UIElement.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Input/KeyboardState.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include <memory>
#include <optional>
#include <string>

namespace Pomdog {
class SpriteFont;
} // namespace Pomdog

namespace Pomdog::UI {

class TextField final
    : public UIElement
    , public std::enable_shared_from_this<TextField> {
public:
    explicit TextField(const std::shared_ptr<UIEventDispatcher>& dispatcher);

    void SetFontWeight(FontWeight fontWeight);

    void SetFontSize(FontSize fontSize);

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

    // void SetTextMargin(const Thickness& margin);
    // void SetMaximumLength(int maxLength);
    // std::string GetSelectedText() const;

    bool HasSelectedText() const;

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
    KeyboardState latestKeys;
    FontWeight fontWeight;
    FontSize fontSize;
    HorizontalAlignment horizontalAlignment;
    bool isEnabled;
    bool isReadOnly;
    bool isAcceptable;
};

} // namespace Pomdog::UI
