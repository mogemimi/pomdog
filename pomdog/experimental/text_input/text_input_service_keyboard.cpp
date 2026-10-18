// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/text_input/text_input_service_keyboard.h"
#include "pomdog/experimental/text_input/text_field_state.h"
#include "pomdog/input/keyboard.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::textinput {
namespace {

[[nodiscard]] bool
isControlByte(char c) noexcept
{
    const auto byte = static_cast<u8>(c);
    return (byte < 0x20) || (byte == 0x7F);
}

} // namespace

TextInputServiceKeyboard::TextInputServiceKeyboard(std::shared_ptr<const Keyboard> keyboard)
    : keyboard_(std::move(keyboard))
{
    POMDOG_ASSERT(keyboard_ != nullptr);
}

TextInputServiceKeyboard::~TextInputServiceKeyboard() = default;

TextInputCapabilities TextInputServiceKeyboard::getCapabilities() const noexcept
{
    TextInputCapabilities capabilities;
    capabilities.committedText = FeatureSupport::Available;
    return capabilities;
}

void TextInputServiceKeyboard::update()
{
    events_.clear();
    if (focusedFieldID_ == 0) {
        return;
    }

    // NOTE: Editing keys such as Backspace and Enter also produce control
    // characters, which the GUI reads as keys instead.
    std::string text;
    for (const auto c : keyboard_->getTextInput()) {
        if (!isControlByte(c)) {
            text.push_back(c);
        }
    }
    if (text.empty()) {
        return;
    }

    TextInputEvent event;
    event.kind = TextInputEventKind::Commit;
    event.fieldID = focusedFieldID_;
    event.text = std::move(text);
    events_.push_back(std::move(event));
}

std::span<const TextInputEvent> TextInputServiceKeyboard::getEvents() const noexcept
{
    return events_;
}

void TextInputServiceKeyboard::setFocusedField(const TextFieldState& field)
{
    POMDOG_ASSERT(field.id != 0);
    focusedFieldID_ = field.id;
}

void TextInputServiceKeyboard::clearFocusedField()
{
    focusedFieldID_ = 0;
}

void TextInputServiceKeyboard::showSoftwareKeyboard()
{
}

void TextInputServiceKeyboard::hideSoftwareKeyboard()
{
}

SoftwareKeyboardState TextInputServiceKeyboard::getSoftwareKeyboardState() const noexcept
{
    return SoftwareKeyboardState{};
}

} // namespace pomdog::textinput
