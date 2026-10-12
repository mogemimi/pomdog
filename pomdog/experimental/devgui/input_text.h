// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/devgui/key.h"
#include "pomdog/experimental/devgui/response.h"

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
/// TODO: Copy and paste need a clipboard API in the engine, and showing text
/// that an input method editor is still composing needs composition events.
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
