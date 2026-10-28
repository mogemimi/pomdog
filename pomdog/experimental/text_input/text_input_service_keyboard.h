// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/text_input/text_input_service.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Keyboard;
} // namespace pomdog

namespace pomdog::textinput {

/// Provides typed text from a `Keyboard`, for platforms without a text input
/// backend.
///
/// The text that `Keyboard::getTextInput()` reports becomes Commit events for
/// the focused field, without control characters. Only committedText is
/// Available; whether an input method editor's text arrives depends on how the
/// platform reports it to the keyboard, and its composition never shows.
class TextInputServiceKeyboard final : public TextInputService {
public:
    /// Creates a service that reads text from keyboard, which must not be null.
    explicit TextInputServiceKeyboard(std::shared_ptr<const Keyboard> keyboard);

    TextInputServiceKeyboard(const TextInputServiceKeyboard&) = delete;
    TextInputServiceKeyboard& operator=(const TextInputServiceKeyboard&) = delete;

    ~TextInputServiceKeyboard() override;

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
    std::shared_ptr<const Keyboard> keyboard_;
    std::vector<TextInputEvent> events_;
    u64 focusedFieldID_ = 0;
};

} // namespace pomdog::textinput
