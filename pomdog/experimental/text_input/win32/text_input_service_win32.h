// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/basic/win32/windows_headers.h"
#include "pomdog/experimental/text_input/text_field_state.h"
#include "pomdog/experimental/text_input/text_input_service.h"
#include "pomdog/math/f32_rect2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class GameWindow;
} // namespace pomdog

namespace pomdog::textinput::detail::win32 {

/// Implements `TextInputService` with the Input Method Manager (IMM32).
///
/// The service subclasses the game window to receive input method messages
/// before the window procedure of the application library. It draws no
/// composition window of its own: the composition reaches the GUI as events,
/// and the input method's candidate window follows the caret area that the GUI
/// reports.
class TextInputServiceWin32 final : public TextInputService {
public:
    TextInputServiceWin32() noexcept;

    ~TextInputServiceWin32() override;

    /// Subclasses windowHandle, the native window of window.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameWindow>& window, HWND windowHandle) noexcept;

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
    setTextFieldAreas(std::span<const TextFieldArea> areas) override;

    void
    showSoftwareKeyboard() override;

    void
    hideSoftwareKeyboard() override;

    [[nodiscard]] SoftwareKeyboardState
    getSoftwareKeyboardState() const noexcept override;

private:
    static LRESULT CALLBACK
    windowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

    /// Handles message and returns its result, or nullopt to pass it to the
    /// previous window procedure.
    [[nodiscard]] std::optional<LRESULT>
    handleMessage(UINT message, WPARAM wParam, LPARAM lParam);

    void
    readComposition(LPARAM flags);

    void
    handleCharacter(char16_t c);

    [[nodiscard]] LRESULT
    queryCharPosition(LPARAM lParam) noexcept;

    void
    pushEvent(TextInputEvent&& event);

    void
    cancelComposition() noexcept;

    void
    setInputMethodEnabled(bool enabled) noexcept;

    void
    applyCaretRect() noexcept;

    /// Returns the caret area in the physical pixels of the client area.
    [[nodiscard]] RECT
    getCaretClientRect() const noexcept;

    /// Restores the previous window procedure when it is still possible.
    void
    detach() noexcept;

private:
    std::shared_ptr<GameWindow> window_;
    HWND windowHandle_ = nullptr;
    WNDPROC previousProcedure_ = nullptr;
    std::vector<TextInputEvent> pendingEvents_;
    std::vector<TextInputEvent> events_;
    std::u16string compositionScratch_;
    std::vector<u8> attributeScratch_;
    F32Rect2D caretRect_ = {0.0f, 0.0f, 0.0f, 0.0f};
    u64 focusedFieldID_ = 0;
    TextInputPurpose purpose_ = TextInputPurpose::Text;

    /// The first half of a surrogate pair from WM_CHAR, which waits for the
    /// second half.
    char16_t pendingHighSurrogate_ = 0;
    bool composing_ = false;
    bool inputMethodEnabled_ = true;
};

} // namespace pomdog::textinput::detail::win32
