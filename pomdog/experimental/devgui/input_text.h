// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/devgui/key.h"
#include "pomdog/experimental/devgui/response.h"
#include "pomdog/experimental/text_input/text_field_state.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
class UI;
} // namespace pomdog::devgui

namespace pomdog::devgui {

/// Describes optional behavior of a text field.
struct InputTextOptions final {
    /// The hint shown in the empty field while it does not have focus.
    std::string_view placeholder = {};

    /// What the field expects. Password draws a dot for each character, and
    /// Integer and Number accept only the characters of numbers, turning the
    /// full-width digits that input methods type into ASCII. The purpose also
    /// reaches the text input service, which can turn the input method editor
    /// off or choose a software keyboard layout.
    textinput::TextInputPurpose purpose = textinput::TextInputPurpose::Text;
};

/// Displays a labeled single-line text field that edits value as UTF-8.
///
/// Clicking the field gives it keyboard focus. While focused, `Response::active`
/// is true, and the field accepts typed text, including text committed by an
/// input method editor, and these keys: the arrow keys, Home, and End move the
/// caret, with shift to select; Backspace and Delete erase; the shortcut
/// modifier with A selects all; Enter keeps the text and Escape restores the
/// text from when focus began. Both, or a press elsewhere, end the focus,
/// which `Response::deactivated` reports. `Response::changed` reports frames
/// where value changed.
///
/// value is changed in place, so it allocates only when it grows. The caret
/// never splits a character from its combining marks.
///
/// With a text input service, the field also shows the text that an input
/// method editor is composing, underlined at the caret, and reports itself
/// to `Context::getFocusedTextField()`. While the input method composes, it
/// owns the editing keys, so that Enter confirms the composition without
/// ending the editing.
///
/// With a clipboard from `Context::setClipboardService()`, the shortcut
/// modifier with C, X, and V copies, cuts, and pastes, and a secondary click
/// opens a menu with the same commands and Select All. A password cannot be
/// copied or cut, and pasted text follows the field's purpose, with line
/// breaks as spaces.
Response
inputText(UI& ui, Key key, std::string_view label, std::string& value, const InputTextOptions& options = {});

/// Reports what the user did with a text field that shows suggestions.
struct SuggestionResponse final {
    /// How the user interacted with the text field.
    Response field = {};

    /// The index of the suggestion the user chose in this frame, if any.
    /// Choosing one also replaces the field's value with it.
    std::optional<std::size_t> chosen;
};

/// Displays a text field like `inputText()` with suggestions in a list below
/// it, for example for a search field.
///
/// The caller decides the suggestions from value each frame, such as the
/// names that contain it, so matching rules stay with the application. When
/// the user changes value, the widget asks for one more frame, so that the
/// suggestions follow the new text without further input. The
/// list shows while the field has focus and suggestions is not empty. The Up
/// and Down keys move a highlight through it, and Enter or a click chooses a
/// suggestion, which replaces value and ends the editing. Escape first closes
/// the list, which opens again when the user types. Enter without a highlight
/// only ends the editing:
/// ```cpp
/// filterNames(names, query, matches);
/// const auto search = devgui::inputTextWithSuggestions(ui, "search"_key, "Search", query, matches);
/// if (search.chosen) {
///     selectByName(query);
/// }
/// ```
SuggestionResponse
inputTextWithSuggestions(
    UI& ui,
    Key key,
    std::string_view label,
    std::string& value,
    std::span<const std::string_view> suggestions,
    const InputTextOptions& options = {});

} // namespace pomdog::devgui

namespace pomdog::devgui::detail {

/// Gives id keyboard focus and starts editing text with all of it selected,
/// for a widget that turns into a text field, such as `dragFloat()` after a
/// click. The widget then calls `editTextInPlace()` instead of drawing itself
/// while `isEditingTextInPlace()` returns true.
void beginTextEdit(UI& ui, WidgetID id, std::string_view text);

/// Returns true while id edits the text that `beginTextEdit()` started,
/// including the frame in which the editing ends because another widget took
/// keyboard focus.
[[nodiscard]] bool
isEditingTextInPlace(UI& ui, WidgetID id) noexcept;

/// Edits the text that `beginTextEdit()` started in field, as `inputText()`
/// edits its value.
///
/// On the frame the editing ends, `Response::deactivated` is set. When the
/// text then differs from the text the editing started with,
/// `Response::changed` is also set and committed receives the text. Escape
/// restores the starting text, so it changes nothing.
Response
editTextInPlace(UI& ui, WidgetID id, const F32Rect2D& field, const InputTextOptions& options, std::string& committed);

} // namespace pomdog::devgui::detail
