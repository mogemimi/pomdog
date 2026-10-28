// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/input_text.h"
#include "pomdog/experimental/clipboard/clipboard_service.h"
#include "pomdog/experimental/devgui/menu.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/text_edit_state.h"
#include "pomdog/experimental/devgui/text_utility.h"
#include "pomdog/experimental/devgui/ui.h"
#include "pomdog/experimental/devgui/widget_helpers.h"
#include "pomdog/experimental/text_input/text_field_state.h"
#include "pomdog/experimental/text_input/text_input_event.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
#include <optional>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
namespace {

/// The time the caret stays visible, and then hidden, while it blinks.
constexpr f64 caretBlinkHalfPeriod = 0.5;

/// Returns the width of each dot that hides a character of a password.
[[nodiscard]] f32
getMaskAdvance(const Style& style) noexcept
{
    return std::round(style.fontSize * 0.6f);
}

[[nodiscard]] bool
isControlByte(char c) noexcept
{
    const auto byte = static_cast<u8>(c);
    return (byte < 0x20) || (byte == 0x7F);
}

void eraseSelection(std::string& value, TextEditState& edit)
{
    const auto start = std::min(edit.caret, edit.anchor);
    const auto end = std::max(edit.caret, edit.anchor);
    value.erase(start, end - start);
    edit.caret = start;
    edit.anchor = start;
}

/// Inserts the printable text of input at the caret, replacing the selection.
/// Returns true when value changed.
[[nodiscard]] bool
insertPrintableText(std::string& value, TextEditState& edit, std::string_view input)
{
    bool changed = false;
    std::size_t runStart = 0;
    for (std::size_t i = 0; i <= input.size(); ++i) {
        if ((i < input.size()) && !isControlByte(input[i])) {
            continue;
        }
        if (i > runStart) {
            if (edit.caret != edit.anchor) {
                eraseSelection(value, edit);
            }
            const auto run = input.substr(runStart, i - runStart);
            value.insert(edit.caret, run);
            edit.caret += static_cast<u32>(run.size());
            edit.anchor = edit.caret;
            changed = true;
        }
        runStart = i + 1;
    }
    return changed;
}

[[nodiscard]] bool
isNumeric(textinput::TextInputPurpose purpose) noexcept
{
    return (purpose == textinput::TextInputPurpose::Integer) || (purpose == textinput::TextInputPurpose::Number);
}

[[nodiscard]] bool
isNumberCharacter(char c, textinput::TextInputPurpose purpose) noexcept
{
    if (((c >= '0') && (c <= '9')) || (c == '+') || (c == '-')) {
        return true;
    }
    return (purpose == textinput::TextInputPurpose::Number) && ((c == '.') || (c == 'e') || (c == 'E'));
}

/// Appends the characters of input that a field for purpose accepts to result.
///
/// Input methods type full-width forms, U+FF01 to U+FF5E, which become their
/// ASCII counterparts.
void appendNumberCharacters(std::string_view input, textinput::TextInputPurpose purpose, std::string& result)
{
    std::size_t i = 0;
    while (i < input.size()) {
        const auto byte = static_cast<u8>(input[i]);
        if (byte < 0x80) {
            if (isNumberCharacter(input[i], purpose)) {
                result.push_back(input[i]);
            }
            i += 1;
            continue;
        }

        // NOTE: U+FF01 to U+FF3F are EF BC 81 to EF BC BF in UTF-8, and U+FF40
        // to U+FF5E are EF BD 80 to EF BD 9E.
        if ((byte == 0xEF) && (i + 2 < input.size())) {
            const auto second = static_cast<u8>(input[i + 1]);
            const auto third = static_cast<u8>(input[i + 2]);
            u32 codePoint = 0;
            if ((second == 0xBC) && (third >= 0x81) && (third <= 0xBF)) {
                codePoint = 0xFF00 + (third - 0x80);
            }
            else if ((second == 0xBD) && (third >= 0x80) && (third <= 0x9E)) {
                codePoint = 0xFF40 + (third - 0x80);
            }
            if (codePoint != 0) {
                const auto ascii = static_cast<char>(codePoint - 0xFEE0);
                if (isNumberCharacter(ascii, purpose)) {
                    result.push_back(ascii);
                }
                i += 3;
                continue;
            }
        }

        // NOTE: Skip any other character as a whole.
        i += 1;
        while ((i < input.size()) && ((static_cast<u8>(input[i]) & 0xC0) == 0x80)) {
            i += 1;
        }
    }
}

/// Inserts the text of input that a field for purpose accepts at the caret,
/// replacing the selection. Returns true when value changed.
[[nodiscard]] bool
insertText(std::string& value, TextEditState& edit, std::string_view input, textinput::TextInputPurpose purpose)
{
    if (!isNumeric(purpose)) {
        return insertPrintableText(value, edit, input);
    }
    std::string accepted;
    appendNumberCharacters(input, purpose, accepted);
    return insertPrintableText(value, edit, accepted);
}

/// Returns the number of characters in text, each with its combining marks.
[[nodiscard]] u32
countCharacters(std::string_view text) noexcept
{
    u32 count = 0;
    u32 offset = 0;
    while (offset < text.size()) {
        offset = detail::nextCharacterBoundary(text, offset);
        count += 1;
    }
    return count;
}

/// Draws text at position, or a dot for each of its characters when masked.
///
/// Text wider than maxWidth ends with an ellipsis, and dots past it are left
/// out. Without maxWidth, the clip rectangle cuts the text instead.
void drawFieldText(
    UI& ui,
    const Vector2& position,
    std::string_view text,
    const Color& color,
    bool masked,
    std::optional<f32> maxWidth)
{
    if (!masked) {
        if (maxWidth) {
            ui.drawText(position, text, color, *maxWidth);
        }
        else {
            ui.drawText(position, text, color);
        }
        return;
    }
    const auto& style = ui.getStyle();
    const auto advance = getMaskAdvance(style);
    const auto radius = style.fontSize * 0.2f;
    const auto count = countCharacters(text);
    for (u32 i = 0; i < count; ++i) {
        const auto left = advance * static_cast<f32>(i);
        if (maxWidth && (left + advance > *maxWidth)) {
            break;
        }
        ui.fillCircle(Vector2{position.x + left + advance * 0.5f, position.y + style.fontSize * 0.55f}, radius, color);
    }
}

/// Moves the caret to position, extending the selection when select is true.
void moveCaret(TextEditState& edit, u32 position, bool select) noexcept
{
    edit.caret = position;
    if (!select) {
        edit.anchor = position;
    }
}

void clearComposition(TextEditState& edit) noexcept
{
    edit.compositionText.clear();
    edit.compositionCaret = 0;
    edit.compositionTargetStart = 0;
    edit.compositionTargetEnd = 0;
    edit.composing = false;
}

/// Deletes before bytes before the selection and after bytes after it, which
/// stays selected. Returns true when value changed.
[[nodiscard]] bool
deleteSurrounding(std::string& value, TextEditState& edit, u32 before, u32 after)
{
    const auto start = std::min(edit.caret, edit.anchor);
    const auto end = std::max(edit.caret, edit.anchor);
    const auto size = static_cast<u32>(value.size());
    const auto deleteStart = detail::clampCharacterBoundary(value, start - std::min(before, start));
    const auto deleteEnd = detail::clampCharacterBoundary(value, end + std::min(after, size - end));

    value.erase(end, deleteEnd - end);
    value.erase(deleteStart, start - deleteStart);
    edit.caret -= start - deleteStart;
    edit.anchor -= start - deleteStart;
    return (deleteStart < start) || (end < deleteEnd);
}

/// Applies the events for the text field id to value. Returns true when an
/// input method composed text in this frame, which then owns the editing keys.
[[nodiscard]] bool
applyTextInputEvents(
    std::span<const textinput::TextInputEvent> events,
    WidgetID id,
    std::string& value,
    TextEditState& edit,
    Response& response,
    textinput::TextInputPurpose purpose,
    f64 timeSeconds)
{
    bool composed = edit.composing;
    for (const auto& event : events) {
        if (event.fieldID != id.value) {
            continue;
        }
        edit.caretMovedTime = timeSeconds;
        switch (event.kind) {
        case textinput::TextInputEventKind::Commit:
            clearComposition(edit);
            if (insertText(value, edit, event.text, purpose)) {
                response.changed = true;
            }
            break;
        case textinput::TextInputEventKind::CompositionUpdate: {
            auto& text = edit.compositionText;
            text.assign(event.text);
            edit.compositionCaret = detail::clampCharacterBoundary(text, event.caret);
            edit.compositionTargetStart = detail::clampCharacterBoundary(text, event.targetStart);
            edit.compositionTargetEnd = std::max(edit.compositionTargetStart, detail::clampCharacterBoundary(text, event.targetEnd));
            edit.composing = true;
            composed = true;
            break;
        }
        case textinput::TextInputEventKind::CompositionEnd:
            clearComposition(edit);
            composed = true;
            break;
        case textinput::TextInputEventKind::DeleteSurrounding:
            if (deleteSurrounding(value, edit, event.deleteBefore, event.deleteAfter)) {
                response.changed = true;
            }
            break;
        }
    }
    return composed;
}

/// Starts editing the field id, which just took keyboard focus, keeping the
/// caret for the caller to place.
void startEditing(TextEditState& edit, WidgetID id, std::string_view value)
{
    edit.id = id;
    edit.scrollX = 0.0f;
    edit.dragUnit = SelectionUnit::Character;
    edit.originalText.assign(value);
    clearComposition(edit);
    edit.pasteRequest = 0;
    edit.cutRequest = 0;
    edit.cutValue.clear();
}

[[nodiscard]] std::string_view
getSelectedText(std::string_view value, const TextEditState& edit) noexcept
{
    const auto start = std::min(edit.caret, edit.anchor);
    const auto end = std::max(edit.caret, edit.anchor);
    return value.substr(start, end - start);
}

/// Returns true when the selection can go to the clipboard. A password never
/// leaves its field.
[[nodiscard]] bool
canCopy(raw_ptr<clipboard::ClipboardService> clipboard, const TextEditState& edit, bool masked) noexcept
{
    return (clipboard != nullptr) && !masked && (edit.caret != edit.anchor);
}

void copySelection(raw_ptr<clipboard::ClipboardService> clipboard, std::string_view value, const TextEditState& edit, bool masked)
{
    if (canCopy(clipboard, edit, masked)) {
        clipboard->requestWriteText(getSelectedText(value, edit));
    }
}

/// Writes the selection to the clipboard and erases it once the write
/// succeeds, so that a failed write loses no text.
void cutSelection(raw_ptr<clipboard::ClipboardService> clipboard, std::string_view value, TextEditState& edit, bool masked)
{
    if (!canCopy(clipboard, edit, masked)) {
        return;
    }
    edit.cutRequest = clipboard->requestWriteText(getSelectedText(value, edit));
    edit.cutValue.assign(value);
    edit.cutStart = std::min(edit.caret, edit.anchor);
    edit.cutEnd = std::max(edit.caret, edit.anchor);
}

/// Asks the clipboard for text to paste. For the paste shortcut, a platform
/// that reports the user's pastes as events, such as a browser, delivers the
/// text by itself; reading as well would paste twice.
void requestPaste(raw_ptr<clipboard::ClipboardService> clipboard, TextEditState& edit, bool shortcut)
{
    if (clipboard == nullptr) {
        return;
    }
    if (shortcut && (clipboard->getCapabilities().pasteEvents == clipboard::FeatureSupport::Available)) {
        return;
    }
    edit.pasteRequest = clipboard->requestReadText();
}

/// Inserts pasted text at the caret, replacing the selection. Line breaks and
/// tabs become spaces, because the field holds a single line. Returns true when
/// value changed.
[[nodiscard]] bool
pasteText(std::string& value, TextEditState& edit, std::string_view text, textinput::TextInputPurpose purpose)
{
    std::string line;
    line.reserve(text.size());
    for (const auto c : text) {
        line.push_back(((c == '\n') || (c == '\r') || (c == '\t')) ? ' ' : c);
    }
    return insertText(value, edit, line, purpose);
}

/// Applies the clipboard events that belong to the focused field: the results
/// of its own requests, and the user's pastes. Returns true when value changed.
[[nodiscard]] bool
applyClipboardEvents(
    raw_ptr<clipboard::ClipboardService> clipboard,
    std::string& value,
    TextEditState& edit,
    textinput::TextInputPurpose purpose)
{
    if (clipboard == nullptr) {
        return false;
    }
    bool changed = false;
    for (const auto& event : clipboard->getEvents()) {
        const bool succeeded = (event.status == clipboard::ClipboardStatus::Succeeded);
        switch (event.kind) {
        case clipboard::ClipboardEventKind::WriteCompleted:
            if ((edit.cutRequest != 0) && (event.requestID == edit.cutRequest)) {
                // NOTE: Erase the cut text only when nothing changed the value
                // or the selection while the clipboard wrote it.
                const bool unchanged = (value == edit.cutValue) &&
                                       (std::min(edit.caret, edit.anchor) == edit.cutStart) &&
                                       (std::max(edit.caret, edit.anchor) == edit.cutEnd);
                if (succeeded && unchanged) {
                    eraseSelection(value, edit);
                    changed = true;
                }
                edit.cutRequest = 0;
                edit.cutValue.clear();
            }
            break;
        case clipboard::ClipboardEventKind::ReadCompleted:
            if ((edit.pasteRequest != 0) && (event.requestID == edit.pasteRequest)) {
                edit.pasteRequest = 0;
                if (succeeded && pasteText(value, edit, event.text, purpose)) {
                    changed = true;
                }
            }
            break;
        case clipboard::ClipboardEventKind::Paste:
            if (succeeded && pasteText(value, edit, event.text, purpose)) {
                changed = true;
            }
            break;
        }
    }
    return changed;
}

using namespace pomdog::devgui::literals;

constexpr Key cutItemKey = "devgui.text.cut"_key;
constexpr Key copyItemKey = "devgui.text.copy"_key;
constexpr Key pasteItemKey = "devgui.text.paste"_key;
constexpr Key selectAllItemKey = "devgui.text.select_all"_key;

constexpr Key suggestionPopupKey = "devgui.suggestions.popup"_key;
constexpr Key suggestionStateKey = "devgui.suggestions.state"_key;
constexpr Key suggestionItemKey = "devgui.suggestions.item"_key;

/// The most suggestions the list shows before it scrolls.
constexpr std::size_t maxVisibleSuggestions = 8;

/// Describes how the popups of a text field, its suggestion list and its
/// context menu, share the field's input.
struct FieldPopups final {
    /// True while the list shows. The field then leaves the Up, Down, and
    /// Escape keys to the list.
    bool listOpen = false;

    /// True while the pointer is over the list, so that a press there keeps
    /// the field's focus until the click chooses a suggestion.
    bool pointerOverList = false;

    /// True while the pointer is over the field's context menu, so that a
    /// press there keeps the field's focus until the click chooses an item.
    bool pointerOverMenu = false;

    bool up = false;
    bool down = false;
    bool escape = false;
};

/// Holds the highlight and whether the user closed the list, in one value.
struct SuggestionState final {
    i32 highlight = -1;
    bool dismissed = false;
};

[[nodiscard]] u64
packSuggestionState(const SuggestionState& state) noexcept
{
    return static_cast<u64>(static_cast<u32>(state.highlight + 1)) | (state.dismissed ? (u64(1) << 32) : u64(0));
}

[[nodiscard]] SuggestionState
unpackSuggestionState(u64 value) noexcept
{
    return SuggestionState{
        .highlight = static_cast<i32>(static_cast<u32>(value & 0xFFFFFFFFULL)) - 1,
        .dismissed = ((value >> 32) & 1) != 0,
    };
}

/// Edits value in the text field id that occupies field.
Response editTextField(UI& ui, WidgetID id, const F32Rect2D& field, std::string& value, const InputTextOptions& options, FieldPopups& keys)
{
    const auto& style = ui.getStyle();
    const auto& input = ui.getInput();
    const auto inner = F32Rect2D{
        field.x + style.framePadding.x,
        field.y,
        std::max(0.0f, field.width - style.framePadding.x * 2.0f),
        field.height,
    };

    const auto pointerResponse = ui.interact(id, field);

    // NOTE: The I-beam shows over the text, and while a drag selects it.
    if (pointerResponse.hovered || pointerResponse.active) {
        ui.setMouseCursor(MouseCursor::IBeam);
    }

    // NOTE: A password shows a dot for each character, so every measure counts
    // characters instead of glyphs.
    const bool masked = (options.purpose == textinput::TextInputPurpose::Password);
    const auto maskAdvance = getMaskAdvance(style);
    const auto measure = [&ui, masked, maskAdvance](std::string_view s) -> f32 {
        if (masked) {
            return static_cast<f32>(countCharacters(s)) * maskAdvance;
        }
        return ui.measureText(s);
    };

    Response response = pointerResponse;
    response.active = false;
    response.activated = false;
    response.deactivated = false;

    bool focused = ui.hasKeyboardFocus(id);
    if (pointerResponse.activated && !focused) {
        ui.setKeyboardFocus(id);
        focused = true;
        response.activated = true;
    }
    auto& edit = ui.getTextEditState(id);
    const bool lostFocus = !focused && (edit.id == id);
    if (focused && (edit.id != id)) {
        // NOTE: Focus was given to this field from elsewhere; start editing.
        startEditing(edit, id, value);
        edit.caret = 0;
        edit.anchor = 0;
    }

    // NOTE: The input method owns the editing keys while it composes text, and
    // in the frame its composition ends, because the keys that confirm or
    // cancel a composition, such as Enter and Escape, also arrive as key
    // presses.
    bool composed = false;
    if (focused || lostFocus) {
        // NOTE: The application can replace value while this field has focus.
        // Keep both offsets on character boundaries before selecting or editing.
        edit.caret = detail::clampCharacterBoundary(value, edit.caret);
        edit.anchor = detail::clampCharacterBoundary(value, edit.anchor);
        composed = edit.composing;
        if (ui.hadKeyboardFocus(id)) {
            composed = applyTextInputEvents(input.textInputEvents, id, value, edit, response, options.purpose, input.timeSeconds);
            if (applyClipboardEvents(ui.getClipboard(), value, edit, options.purpose)) {
                response.changed = true;
                edit.caretMovedTime = input.timeSeconds;
            }
        }
    }

    const auto pointerX = input.pointerPosition.x - inner.x + edit.scrollX;
    if (pointerResponse.activated) {
        // NOTE: The input method keeps composing at the caret, so the caret
        // stays where the composition began.
        if (!edit.composing) {
            if (pointerResponse.tripleClicked || (pointerResponse.doubleClicked && masked)) {
                // NOTE: A triple click selects the line, which is the whole
                // text of a single-line field. A password hides its words, so
                // a double click selects all of it too.
                edit.anchor = 0;
                edit.caret = static_cast<u32>(value.size());
                edit.dragUnit = SelectionUnit::All;
            }
            else if (pointerResponse.doubleClicked) {
                const auto word = detail::findWordAt(value, detail::findCharacterAt(measure, value, pointerX));
                edit.anchor = word.start;
                edit.caret = word.end;
                edit.dragUnit = SelectionUnit::Word;
                edit.dragWordStart = word.start;
                edit.dragWordEnd = word.end;
            }
            else {
                const auto position = detail::findCharacterBoundaryAt(measure, value, pointerX);
                moveCaret(edit, position, input.shiftDown && !response.activated);
                edit.dragUnit = SelectionUnit::Character;
            }
            edit.caretMovedTime = input.timeSeconds;
        }
    }
    else if (focused && pointerResponse.active && !edit.composing) {
        // NOTE: Dragging with the pointer held extends the selection, by whole
        // words after a double click. The double-clicked word stays selected.
        switch (edit.dragUnit) {
        case SelectionUnit::Character:
            edit.caret = detail::findCharacterBoundaryAt(measure, value, pointerX);
            break;
        case SelectionUnit::Word: {
            const auto word = detail::findWordAt(value, detail::findCharacterAt(measure, value, pointerX));
            edit.anchor = (word.start < edit.dragWordStart) ? edit.dragWordEnd : edit.dragWordStart;
            edit.caret = (word.start < edit.dragWordStart) ? word.start : std::max(word.end, edit.dragWordEnd);
            break;
        }
        case SelectionUnit::All:
            break;
        }
    }
    else if (lostFocus || (focused && ui.isPointerPressed() && !keys.pointerOverList && !keys.pointerOverMenu)) {
        // NOTE: A press anywhere else ends the editing and keeps the text,
        // including the text being composed, which the user already sees. The
        // text input service cancels the composition when the focus ends.
        if (edit.composing && insertText(value, edit, edit.compositionText, options.purpose)) {
            response.changed = true;
        }
        clearComposition(edit);
        if (focused) {
            ui.clearKeyboardFocus();
        }
        focused = false;
        response.deactivated = true;
    }

    if (focused && !composed) {
        const auto caretBefore = edit.caret;
        const auto anchorBefore = edit.anchor;
        const bool select = input.shiftDown;
        const bool hasSelection = (edit.caret != edit.anchor);

        if (keys.listOpen && ui.isKeyPressed(EditKey::Escape)) {
            keys.escape = true;
        }
        else if (ui.isKeyPressed(EditKey::Escape)) {
            if (value != edit.originalText) {
                value.assign(edit.originalText);
                response.changed = true;
            }
            ui.clearKeyboardFocus();
            focused = false;
            response.deactivated = true;
        }
        else if (ui.isKeyPressed(EditKey::Enter)) {
            ui.clearKeyboardFocus();
            focused = false;
            response.deactivated = true;
        }
        else {
            if (input.shortcutDown && ui.isKeyPressed(EditKey::A)) {
                edit.anchor = 0;
                edit.caret = static_cast<u32>(value.size());
            }
            if (input.shortcutDown && ui.isKeyPressed(EditKey::C)) {
                copySelection(ui.getClipboard(), value, edit, masked);
            }
            if (input.shortcutDown && ui.isKeyPressed(EditKey::X)) {
                cutSelection(ui.getClipboard(), value, edit, masked);
            }
            if (input.shortcutDown && ui.isKeyPressed(EditKey::V)) {
                requestPaste(ui.getClipboard(), edit, true);
            }
            if (ui.isKeyPressed(EditKey::LeftArrow)) {
                if (hasSelection && !select) {
                    moveCaret(edit, std::min(edit.caret, edit.anchor), false);
                }
                else {
                    moveCaret(edit, detail::previousCharacterBoundary(value, edit.caret), select);
                }
            }
            if (ui.isKeyPressed(EditKey::RightArrow)) {
                if (hasSelection && !select) {
                    moveCaret(edit, std::max(edit.caret, edit.anchor), false);
                }
                else {
                    moveCaret(edit, detail::nextCharacterBoundary(value, edit.caret), select);
                }
            }
            if (keys.listOpen) {
                keys.up = ui.isKeyPressed(EditKey::UpArrow);
                keys.down = ui.isKeyPressed(EditKey::DownArrow);
            }
            if (ui.isKeyPressed(EditKey::Home) || (!keys.listOpen && ui.isKeyPressed(EditKey::UpArrow))) {
                moveCaret(edit, 0, select);
            }
            if (ui.isKeyPressed(EditKey::End) || (!keys.listOpen && ui.isKeyPressed(EditKey::DownArrow))) {
                moveCaret(edit, static_cast<u32>(value.size()), select);
            }
            if (ui.isKeyPressed(EditKey::Backspace)) {
                if (edit.caret != edit.anchor) {
                    eraseSelection(value, edit);
                    response.changed = true;
                }
                else if (edit.caret > 0) {
                    const auto start = detail::previousCharacterBoundary(value, edit.caret);
                    value.erase(start, edit.caret - start);
                    moveCaret(edit, start, false);
                    response.changed = true;
                }
            }
            if (ui.isKeyPressed(EditKey::Delete)) {
                if (edit.caret != edit.anchor) {
                    eraseSelection(value, edit);
                    response.changed = true;
                }
                else if (edit.caret < value.size()) {
                    const auto end = detail::nextCharacterBoundary(value, edit.caret);
                    value.erase(edit.caret, end - edit.caret);
                    response.changed = true;
                }
            }
            if (!input.shortcutDown && insertText(value, edit, input.textInput, options.purpose)) {
                response.changed = true;
            }
        }

        if (response.changed || (edit.caret != caretBefore) || (edit.anchor != anchorBefore)) {
            edit.caretMovedTime = input.timeSeconds;
        }
    }
    response.active = focused;
    if (response.deactivated) {
        edit.id = WidgetID{};
    }

    const auto border = detail::getInputBorderColor(style, pointerResponse, focused);
    detail::drawFrame(ui, field, style.inputBackgroundColor, border, style.frameRounding);
    const auto textTop = detail::getTextTop(style, field);

    if (!focused) {
        if (value.empty()) {
            ui.drawText(Vector2{inner.x, textTop}, options.placeholder, style.textSecondaryColor, inner.width);
        }
        else {
            drawFieldText(ui, Vector2{inner.x, textTop}, value, style.textColor, masked, inner.width);
        }
        return response;
    }

    // NOTE: The text being composed shows in place of the selection, without
    // becoming part of value.
    const std::string_view text = value;
    const auto selectionStart = std::min(edit.caret, edit.anchor);
    const auto selectionEnd = std::max(edit.caret, edit.anchor);
    const std::string_view composition = edit.compositionText;
    const auto beforeWidth = measure(text.substr(0, selectionStart));
    const auto compositionWidth = measure(composition);

    f32 caretX = 0.0f;
    f32 textWidth = 0.0f;
    f32 targetX = 0.0f;
    if (edit.composing) {
        caretX = beforeWidth + measure(composition.substr(0, edit.compositionCaret));
        textWidth = beforeWidth + compositionWidth + measure(text.substr(selectionEnd));
        targetX = caretX;
        if (edit.compositionTargetStart < edit.compositionTargetEnd) {
            targetX = beforeWidth + measure(composition.substr(0, edit.compositionTargetStart));
        }
    }
    else {
        caretX = measure(text.substr(0, edit.caret));
        textWidth = measure(text);
        targetX = caretX;
    }

    // NOTE: Scroll the text so the caret stays inside the field.
    if (caretX - edit.scrollX > inner.width) {
        edit.scrollX = caretX - inner.width;
    }
    if (caretX < edit.scrollX) {
        edit.scrollX = caretX;
    }
    edit.scrollX = std::clamp(edit.scrollX, 0.0f, std::max(0.0f, textWidth - inner.width + 1.0f));

    ui.pushClipRect(F32Rect2D{inner.x - 1.0f, field.y, inner.width + 2.0f, field.height});
    const auto originX = inner.x - edit.scrollX;
    if (edit.composing) {
        // NOTE: A thin line underlines the composition, and a thick one the
        // part the input method is converting.
        const auto compositionX = originX + beforeWidth;
        const auto afterX = compositionX + compositionWidth;
        drawFieldText(ui, Vector2{originX, textTop}, text.substr(0, selectionStart), style.textColor, masked, std::nullopt);
        drawFieldText(ui, Vector2{compositionX, textTop}, composition, style.textColor, masked, std::nullopt);
        drawFieldText(ui, Vector2{afterX, textTop}, text.substr(selectionEnd), style.textColor, masked, std::nullopt);

        const auto underlineY = textTop + style.fontSize + 1.0f;
        ui.fillRect(F32Rect2D{std::round(compositionX), underlineY, std::round(compositionWidth), 1.0f}, style.textColor);
        if (edit.compositionTargetStart < edit.compositionTargetEnd) {
            const auto targetEndX = beforeWidth + measure(composition.substr(0, edit.compositionTargetEnd));
            const auto target = F32Rect2D{std::round(originX + targetX), underlineY, std::round(targetEndX - targetX), 2.0f};
            ui.fillRect(target, style.textColor);
        }
    }
    else {
        if (selectionStart != selectionEnd) {
            const auto endX = measure(text.substr(0, selectionEnd));
            const auto selection = F32Rect2D{originX + beforeWidth, field.y + 3.0f, endX - beforeWidth, field.height - 6.0f};
            ui.fillRect(selection, style.selectionColor);
        }
        drawFieldText(ui, Vector2{originX, textTop}, text, style.textColor, masked, std::nullopt);
    }

    // NOTE: The caret blinks relative to its last move, and asks for a frame at
    // each change between visible and hidden.
    const auto elapsed = std::max(0.0, input.timeSeconds - edit.caretMovedTime);
    const auto phase = static_cast<i64>(std::floor(elapsed / caretBlinkHalfPeriod));
    if ((phase % 2) == 0) {
        const auto x = std::round(originX + caretX);
        ui.fillRect(F32Rect2D{x, field.y + 4.0f, 1.0f, field.height - 8.0f}, style.textColor);
    }
    ui.requestUpdateAt(edit.caretMovedTime + static_cast<f64>(phase + 1) * caretBlinkHalfPeriod);
    ui.popClipRect();

    // NOTE: The input method shows its candidate window next to the caret, or
    // next to the part it is converting.
    // A password never leaves the field.
    textinput::TextFieldState state;
    state.id = id.value;
    state.purpose = options.purpose;
    state.caretRect = F32Rect2D{std::clamp(std::round(originX + targetX), inner.x, inner.x + inner.width), field.y, 1.0f, field.height};
    if (!masked) {
        state.surroundingText = text;
        state.caret = edit.caret;
        state.anchor = edit.anchor;
    }
    ui.setFocusedTextField(state);
    return response;
}

/// Builds the context menu of the text field that response describes, with
/// Cut, Copy, Paste, and Select All.
void buildContextMenu(UI& ui, Response& response, std::string& value, const InputTextOptions& options)
{
    if (response.secondaryClicked && !ui.hasKeyboardFocus(response.id)) {
        // NOTE: The menu works on the field's selection, so the field takes
        // focus first, with the caret at the end.
        ui.setKeyboardFocus(response.id);
        auto& edit = ui.getTextEditState(response.id);
        startEditing(edit, response.id, value);
        edit.caret = static_cast<u32>(value.size());
        edit.anchor = edit.caret;
        response.activated = true;
        response.active = true;
    }

    auto& edit = ui.getTextEditState(response.id);
    auto menu = contextMenu(ui, response);
    if (!menu.isOpen() || (edit.id != response.id)) {
        return;
    }

    const auto clipboard = ui.getClipboard();
    const bool masked = (options.purpose == textinput::TextInputPurpose::Password);
    const bool copyable = canCopy(clipboard, edit, masked);
    const bool usesSuper = ui.getInput().shortcutUsesSuper;
    const std::string_view cutShortcut = usesSuper ? "Cmd+X" : "Ctrl+X";
    const std::string_view copyShortcut = usesSuper ? "Cmd+C" : "Ctrl+C";
    const std::string_view pasteShortcut = usesSuper ? "Cmd+V" : "Ctrl+V";
    const std::string_view selectAllShortcut = usesSuper ? "Cmd+A" : "Ctrl+A";
    if (menuItem(ui, cutItemKey, "Cut", {.shortcut = cutShortcut, .checked = false, .enabled = copyable}).clicked) {
        cutSelection(clipboard, value, edit, masked);
    }
    if (menuItem(ui, copyItemKey, "Copy", {.shortcut = copyShortcut, .checked = false, .enabled = copyable}).clicked) {
        copySelection(clipboard, value, edit, masked);
    }

    // NOTE: A browser reports pastes only for its own paste actions, so the
    // menu always reads the clipboard, which may ask the user for permission.
    if (menuItem(ui, pasteItemKey, "Paste", {.shortcut = pasteShortcut, .checked = false, .enabled = (clipboard != nullptr)}).clicked) {
        requestPaste(clipboard, edit, false);
    }
    menuSeparator(ui);
    if (menuItem(ui, selectAllItemKey, "Select All", {.shortcut = selectAllShortcut, .checked = false, .enabled = !value.empty()}).clicked) {
        edit.anchor = 0;
        edit.caret = static_cast<u32>(value.size());
        edit.caretMovedTime = ui.getInput().timeSeconds;
    }
}

} // namespace

Response inputText(UI& ui, Key key, std::string_view label, std::string& value, const InputTextOptions& options)
{
    const auto id = ui.makeID(key);
    const auto row = detail::layoutPropertyRow(ui, label);
    FieldPopups keys;
    keys.pointerOverMenu = ui.isPointerOverPopup(getContextMenuID(id));
    auto response = editTextField(ui, id, row.control, value, options, keys);
    buildContextMenu(ui, response, value, options);
    return response;
}

SuggestionResponse inputTextWithSuggestions(
    UI& ui,
    Key key,
    std::string_view label,
    std::string& value,
    std::span<const std::string_view> suggestions,
    const InputTextOptions& options)
{
    const auto& style = ui.getStyle();
    const auto id = ui.makeID(key);
    const auto row = detail::layoutPropertyRow(ui, label);
    const auto& field = row.control;
    const auto popupID = combineID(id, suggestionPopupKey);
    const auto stateID = combineID(id, suggestionStateKey);

    SuggestionState state;
    if (const auto stored = ui.loadState(stateID); stored) {
        state = unpackSuggestionState(*stored);
    }
    const auto count = static_cast<i32>(suggestions.size());

    FieldPopups keys;
    keys.listOpen = ui.hasKeyboardFocus(id) && !state.dismissed && (count > 0) && ui.isPopupOpen(popupID);
    keys.pointerOverList = keys.listOpen && ui.isPointerOverPopup(popupID);
    keys.pointerOverMenu = ui.isPointerOverPopup(getContextMenuID(id));

    SuggestionResponse result;
    result.field = editTextField(ui, id, field, value, options, keys);
    auto& response = result.field;
    buildContextMenu(ui, response, value, options);

    if (response.activated || response.changed) {
        // NOTE: Typing shows the list again with no highlight.
        state = SuggestionState{};
    }
    if (response.changed) {
        // NOTE: The caller made suggestions from the previous text. Ask for a
        // frame that shows the suggestions for the new text.
        ui.requestUpdateAt(ui.getInput().timeSeconds);
    }
    if (keys.escape) {
        state.dismissed = true;
    }
    bool highlightMoved = false;
    if (keys.down) {
        state.highlight += 1;
        highlightMoved = true;
    }
    if (keys.up) {
        state.highlight -= 1;
        highlightMoved = true;
    }
    state.highlight = std::clamp(state.highlight, -1, count - 1);

    const auto choose = [&](i32 index) {
        value.assign(suggestions[static_cast<std::size_t>(index)]);
        result.chosen = static_cast<std::size_t>(index);
        response.changed = true;
        state = SuggestionState{};

        // NOTE: The field was drawn before the choice. Ask for a frame that
        // shows the chosen text.
        ui.requestUpdateAt(ui.getInput().timeSeconds);
    };

    // NOTE: Enter ended the editing in the field. With a highlight it also
    // chooses the highlighted suggestion.
    if (keys.listOpen && response.deactivated && ui.isKeyPressed(EditKey::Enter) && (state.highlight >= 0)) {
        choose(state.highlight);
    }

    // NOTE: Opening the list would close the field's context menu, which
    // shares the top level of the popup stack.
    const bool showList = ui.hasKeyboardFocus(id) && !state.dismissed && (count > 0) && !ui.isPopupOpen(getContextMenuID(id));
    if (showList) {
        // NOTE: A press outside the list also reaches the field, so that
        // clicking the field moves the caret while the list stays open.
        ui.openPopup(popupID, PopupOutsidePress::PassThrough);
    }
    if (showList && ui.isPopupOpen(popupID)) {
        const auto rowHeight = style.getFrameHeight();
        const auto visibleRows = static_cast<f32>(std::min(suggestions.size(), maxVisibleSuggestions));
        const auto listHeight = rowHeight * visibleRows + style.itemSpacing.y * (visibleRows - 1.0f) + style.popupPadding.y * 2.0f;
        ui.beginPopup(popupID, field, Vector2{field.width, listHeight});
        for (i32 i = 0; i < count; ++i) {
            const auto rect = ui.allocate(0.0f, rowHeight);
            const auto itemResponse = ui.interact(ui.makeID(suggestionItemKey.at(static_cast<u32>(i))), rect);
            if (itemResponse.hovered && (ui.getPointerDelta() != Vector2{0.0f, 0.0f})) {
                // NOTE: Moving the pointer onto a suggestion highlights it, but
                // a list that scrolled under a resting pointer does not.
                state.highlight = i;
            }
            const bool highlighted = (i == state.highlight);
            if (highlighted) {
                ui.fillRect(rect, style.rowSelectedColor, style.frameRounding);
                if (highlightMoved) {
                    ui.scrollIntoView(rect);
                }
            }
            const auto textX = rect.x + style.framePadding.x;
            const auto text = suggestions[static_cast<std::size_t>(i)];
            const auto textColor = highlighted ? style.textOnAccentColor : style.textColor;
            ui.drawText(Vector2{textX, detail::getTextTop(style, rect)}, text, textColor, std::max(0.0f, rect.x + rect.width - style.framePadding.x - textX));
            if (itemResponse.clicked) {
                choose(i);
                ui.clearKeyboardFocus();
                response.active = false;
                response.deactivated = true;
                ui.closePopup();
            }
        }
        ui.endPopup();
    }

    ui.storeState(stateID, packSuggestionState(state));
    return result;
}

} // namespace pomdog::devgui

namespace pomdog::devgui::detail {

void beginTextEdit(UI& ui, WidgetID id, std::string_view text)
{
    ui.setKeyboardFocus(id);
    auto& edit = ui.getTextEditState(id);
    startEditing(edit, id, text);
    edit.editedText.assign(text);

    // NOTE: Select the whole text, so that typing replaces it.
    edit.anchor = 0;
    edit.caret = static_cast<u32>(text.size());
    edit.caretMovedTime = ui.getInput().timeSeconds;
    ui.requestUpdateAt(ui.getInput().timeSeconds);
}

bool isEditingTextInPlace(UI& ui, WidgetID id) noexcept
{
    return ui.hasKeyboardFocus(id) || (ui.getTextEditState(id).id == id);
}

Response editTextInPlace(UI& ui, WidgetID id, const F32Rect2D& field, const InputTextOptions& options, std::string& committed)
{
    auto& edit = ui.getTextEditState(id);
    FieldPopups keys;
    keys.pointerOverMenu = ui.isPointerOverPopup(getContextMenuID(id));
    auto response = editTextField(ui, id, field, edit.editedText, options, keys);
    buildContextMenu(ui, response, edit.editedText, options);

    // NOTE: The text changes while the user types, but the caller's value
    // changes only when the editing ends.
    response.changed = false;
    if (response.deactivated && (edit.editedText != edit.originalText)) {
        committed.assign(edit.editedText);
        response.changed = true;
    }
    return response;
}

} // namespace pomdog::devgui::detail
