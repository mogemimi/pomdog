// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/text_input/text_field_state.h"
#include "pomdog/experimental/text_input/text_input_service.h"
#include "pomdog/math/f32_rect2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::textinput::detail::emscripten {

/// Implements `TextInputService` with a hidden HTML input element.
///
/// A canvas cannot receive text, so the service focuses an invisible input
/// element at the caret while a field has focus. The browser's input method
/// composes into that element, its composition events become events for the
/// GUI, and its candidate window opens at the element. The element stays empty
/// between compositions; editing keys still reach the GUI as keys.
class TextInputServiceEmscripten final : public TextInputService {
public:
    TextInputServiceEmscripten() noexcept;

    TextInputServiceEmscripten(const TextInputServiceEmscripten&) = delete;
    TextInputServiceEmscripten& operator=(const TextInputServiceEmscripten&) = delete;

    ~TextInputServiceEmscripten() override;

    /// Creates the input element next to the canvas that canvasSelector names.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::string& canvasSelector) noexcept;

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
    /// Moves the events that the input element queued to pendingEvents_,
    /// addressed to fieldID. Events are dropped when fieldID is zero.
    void
    takeEvents(u64 fieldID);

private:
    std::vector<TextInputEvent> pendingEvents_;
    std::vector<TextInputEvent> events_;
    u64 focusedFieldID_ = 0;
    TextInputPurpose purpose_ = TextInputPurpose::Text;
    bool initialized_ = false;
};

} // namespace pomdog::textinput::detail::emscripten
