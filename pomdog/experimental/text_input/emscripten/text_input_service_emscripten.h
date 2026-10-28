// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/text_input/text_field_state.h"
#include "pomdog/experimental/text_input/text_input_service.h"
#include "pomdog/experimental/text_input/text_mirror.h"
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
    setTextFieldAreas(std::span<const TextFieldArea> areas) override;

    void
    showSoftwareKeyboard() override;

    void
    hideSoftwareKeyboard() override;

    [[nodiscard]] SoftwareKeyboardState
    getSoftwareKeyboardState() const noexcept override;

private:
    /// Turns the states that the input element queued into events for fieldID
    /// and appends them to pendingEvents_. The states are dropped when fieldID
    /// is zero.
    void
    takeEvents(u64 fieldID);

private:
    std::vector<TextInputEvent> pendingEvents_;
    std::vector<TextInputEvent> events_;

    // NOTE: The text and selection that the element shows for the focused
    // field. elementText_ and mirrorEvents_ hold each state the element
    // reports while it turns into events.
    TextMirror mirror_;
    std::string elementText_;
    std::vector<TextInputEvent> mirrorEvents_;

    // NOTE: The areas that the input element knows, five numbers for each:
    // the rectangle and the purpose. areaScratch_ builds the next ones.
    std::vector<f32> areaData_;
    std::vector<f32> areaScratch_;
    u64 focusedFieldID_ = 0;

    // NOTE: True while the element stays focused after the GUI let its field
    // go, in case a tap gives the focus to another field.
    bool blurPending_ = false;
    TextInputPurpose purpose_ = TextInputPurpose::Text;
    bool initialized_ = false;
};

} // namespace pomdog::textinput::detail::emscripten
