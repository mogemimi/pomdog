// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/text_input/composition_buffer.h"
#include "pomdog/experimental/text_input/text_field_state.h"
#include "pomdog/experimental/text_input/text_input_service.h"
#include "pomdog/math/f32_rect2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <X11/Xlib.h>
#include <locale.h>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail::x11 {
class GameWindowX11;
} // namespace pomdog::detail::x11

namespace pomdog::textinput::detail::x11 {

/// Implements `TextInputService` with the X Input Method (XIM).
///
/// The service opens its own connection to the user's input method, which
/// XMODIFIERS names, such as fcitx or ibus. Its input context has focus only
/// while a field that uses the input method has focus; otherwise the window's
/// own input context keeps handling the keyboard's text. It prefers the
/// on-the-spot style, where preedit callbacks report the composition for the
/// GUI to draw, and falls back to the over-the-spot style, where the input
/// method draws it at the caret, and then to committed text alone.
class TextInputServiceX11 final : public TextInputService {
public:
    TextInputServiceX11() noexcept;

    TextInputServiceX11(const TextInputServiceX11&) = delete;
    TextInputServiceX11& operator=(const TextInputServiceX11&) = delete;

    ~TextInputServiceX11() override;

    /// Connects to the input method and filters the events of window.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<pomdog::detail::x11::GameWindowX11>& window) noexcept;

    [[nodiscard]] TextInputCapabilities
    getCapabilities() const noexcept override;

    void
    update() override;

    [[nodiscard]] std::span<const TextInputEvent>
    getEvents() const noexcept override;

    void
    setFocusedField(const TextFieldState& field) override;

    void
    clearFocusedField() override;

    void
    showSoftwareKeyboard() override;

    void
    hideSoftwareKeyboard() override;

    [[nodiscard]] SoftwareKeyboardState
    getSoftwareKeyboardState() const noexcept override;

private:
    static int
    onPreeditStart(::XIC inputContext, ::XPointer clientData, ::XPointer callData);

    static void
    onPreeditDone(::XIM inputMethod, ::XPointer clientData, ::XPointer callData);

    static void
    onPreeditDraw(::XIM inputMethod, ::XPointer clientData, ::XPointer callData);

    static void
    onPreeditCaret(::XIM inputMethod, ::XPointer clientData, ::XPointer callData);

    /// Opens the input method in the environment's locale, with the font set
    /// that the over-the-spot style needs. Sets utf8 to whether that locale
    /// encodes text in UTF-8.
    void
    openInputMethod(bool& utf8) noexcept;

    /// Creates the input context with the first style that the input method
    /// accepts.
    [[nodiscard]] bool
    createInputContext(bool utf8) noexcept;

    /// Runs the input method's filters on event, and returns true when they
    /// consumed it.
    [[nodiscard]] bool
    filterEvent(::XEvent& event);

    void
    lookUpCommittedText(::XKeyEvent& event);

    void
    pushCompositionUpdate();

    void
    pushEvent(TextInputEvent&& event);

    void
    endComposition();

    /// Discards the composition in the input method.
    void
    resetComposition() noexcept;

    /// Gives focus to the input context while a field that uses the input
    /// method has focus in a focused window, and takes it away otherwise.
    void
    updateFocus() noexcept;

    /// Places the input method's candidate window at the caret.
    void
    applySpotLocation() noexcept;

private:
    std::shared_ptr<pomdog::detail::x11::GameWindowX11> window_;
    ::Display* display_ = nullptr;
    ::Window windowHandle_ = 0;
    ::XIM inputMethod_ = nullptr;
    ::XIC inputContext_ = nullptr;
    ::XIMStyle inputStyle_ = 0;
    ::XFontSet fontSet_ = nullptr;

    // NOTE: Xlib counts the characters of preedit text with mblen(), which
    // reads the thread's locale. The "C" locale miscounts UTF-8 text, so the
    // calls that can run the preedit callbacks switch the thread to this
    // locale, the environment's, which the input method opened with.
    ::locale_t inputLocale_ = {};

    // NOTE: The input context keeps pointers to these, so they live as long as
    // the service.
    ::XICCallback preeditStartCallback_ = {};
    ::XIMCallback preeditDoneCallback_ = {};
    ::XIMCallback preeditDrawCallback_ = {};
    ::XIMCallback preeditCaretCallback_ = {};

    CompositionBuffer composition_;
    std::vector<TextInputEvent> pendingEvents_;
    std::vector<TextInputEvent> events_;
    std::vector<char> lookupBuffer_;
    F32Rect2D caretRect_ = {0.0f, 0.0f, 0.0f, 0.0f};
    u64 focusedFieldID_ = 0;
    TextInputPurpose purpose_ = TextInputPurpose::Text;
    bool composing_ = false;
    bool inputContextFocused_ = false;
    bool windowFocused_ = true;
};

} // namespace pomdog::textinput::detail::x11
