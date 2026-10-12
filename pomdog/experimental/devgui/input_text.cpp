// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/input_text.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/text_edit_state.h"
#include "pomdog/experimental/devgui/text_utility.h"
#include "pomdog/experimental/devgui/ui.h"
#include "pomdog/experimental/devgui/widget_helpers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
namespace {

/// The time the caret stays visible, and then hidden, while it blinks.
constexpr f64 caretBlinkHalfPeriod = 0.5;

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
insertText(std::string& value, TextEditState& edit, std::string_view input)
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

/// Moves the caret to position, extending the selection when select is true.
void moveCaret(TextEditState& edit, u32 position, bool select) noexcept
{
    edit.caret = position;
    if (!select) {
        edit.anchor = position;
    }
}

using namespace pomdog::devgui::literals;

constexpr Key suggestionPopupKey = "devgui.suggestions.popup"_key;
constexpr Key suggestionStateKey = "devgui.suggestions.state"_key;
constexpr Key suggestionItemKey = "devgui.suggestions.item"_key;

/// The most suggestions the list shows before it scrolls.
constexpr std::size_t maxVisibleSuggestions = 8;

/// Describes how a suggestion list shares the keys of its text field.
struct SuggestionKeys final {
    /// True while the list shows. The field then leaves the Up, Down, and
    /// Escape keys to the list.
    bool listOpen = false;

    /// True while the pointer is over the list, so that a press there keeps
    /// the field's focus until the click chooses a suggestion.
    bool pointerOverList = false;

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
Response editTextField(UI& ui, WidgetID id, const F32Rect2D& field, std::string& value, const InputTextOptions& options, SuggestionKeys& keys)
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
    auto& edit = ui.getTextEditState();
    const auto measure = [&ui](std::string_view s) -> f32 { return ui.measureText(s); };

    Response response = pointerResponse;
    response.active = false;
    response.activated = false;
    response.deactivated = false;

    bool focused = ui.hasKeyboardFocus(id);
    if (focused && (edit.id != id)) {
        // NOTE: Focus was given to this field from elsewhere; start editing.
        edit = TextEditState{};
        edit.id = id;
        edit.originalText.assign(value);
    }

    if (focused) {
        // NOTE: The application can replace value while this field has focus.
        // Keep both offsets on character boundaries before selecting or editing.
        edit.caret = detail::clampCharacterBoundary(value, edit.caret);
        edit.anchor = detail::clampCharacterBoundary(value, edit.anchor);
    }

    if (pointerResponse.activated) {
        if (!focused) {
            ui.setKeyboardFocus(id);
            edit.id = id;
            edit.scrollX = 0.0f;
            edit.originalText.assign(value);
            focused = true;
            response.activated = true;
        }
        const auto position = detail::findCharacterBoundaryAt(measure, value, input.pointerPosition.x - inner.x + edit.scrollX);
        moveCaret(edit, position, input.shiftDown && !response.activated);
        edit.caretMovedTime = input.timeSeconds;
    }
    else if (focused && pointerResponse.active) {
        // NOTE: Dragging with the pointer held extends the selection.
        edit.caret = detail::findCharacterBoundaryAt(measure, value, input.pointerPosition.x - inner.x + edit.scrollX);
    }
    else if (focused && ui.isPointerPressed() && !keys.pointerOverList) {
        // NOTE: A press anywhere else ends the editing and keeps the text.
        ui.clearKeyboardFocus();
        focused = false;
        response.deactivated = true;
    }

    if (focused) {
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
            if (!input.shortcutDown && insertText(value, edit, input.textInput)) {
                response.changed = true;
            }
        }

        if (response.changed || (edit.caret != caretBefore) || (edit.anchor != anchorBefore)) {
            edit.caretMovedTime = input.timeSeconds;
        }
    }
    response.active = focused;

    const auto border = detail::getInputBorderColor(style, pointerResponse, focused);
    detail::drawFrame(ui, field, style.inputBackgroundColor, border, style.frameRounding);
    const auto textTop = detail::getTextTop(style, field);

    if (!focused) {
        if (value.empty()) {
            ui.drawText(Vector2{inner.x, textTop}, options.placeholder, style.textSecondaryColor, inner.width);
        }
        else {
            ui.drawText(Vector2{inner.x, textTop}, value, style.textColor, inner.width);
        }
        return response;
    }

    // NOTE: Scroll the text so the caret stays inside the field.
    const auto caretX = measure(std::string_view{value}.substr(0, edit.caret));
    const auto textWidth = measure(value);
    if (caretX - edit.scrollX > inner.width) {
        edit.scrollX = caretX - inner.width;
    }
    if (caretX < edit.scrollX) {
        edit.scrollX = caretX;
    }
    edit.scrollX = std::clamp(edit.scrollX, 0.0f, std::max(0.0f, textWidth - inner.width + 1.0f));

    ui.pushClipRect(F32Rect2D{inner.x - 1.0f, field.y, inner.width + 2.0f, field.height});
    const auto originX = inner.x - edit.scrollX;
    if (edit.caret != edit.anchor) {
        const auto start = std::min(edit.caret, edit.anchor);
        const auto end = std::max(edit.caret, edit.anchor);
        const auto startX = measure(std::string_view{value}.substr(0, start));
        const auto endX = measure(std::string_view{value}.substr(0, end));
        const auto selection = F32Rect2D{originX + startX, field.y + 3.0f, endX - startX, field.height - 6.0f};
        ui.fillRect(selection, style.selectionColor);
    }
    ui.drawText(Vector2{originX, textTop}, value, style.textColor);

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
    return response;
}

} // namespace

Response inputText(UI& ui, Key key, std::string_view label, std::string& value, const InputTextOptions& options)
{
    const auto id = ui.makeID(key);
    const auto row = detail::layoutPropertyRow(ui, label);
    SuggestionKeys keys;
    return editTextField(ui, id, row.control, value, options, keys);
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

    SuggestionKeys keys;
    keys.listOpen = ui.hasKeyboardFocus(id) && !state.dismissed && (count > 0) && ui.isPopupOpen(popupID);
    keys.pointerOverList = keys.listOpen && ui.isPointerOverPopup(popupID);

    SuggestionResponse result;
    result.field = editTextField(ui, id, field, value, options, keys);
    auto& response = result.field;

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

    const bool showList = ui.hasKeyboardFocus(id) && !state.dismissed && (count > 0);
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
