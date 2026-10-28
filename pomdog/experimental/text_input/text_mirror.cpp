// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/text_input/text_mirror.h"
#include "pomdog/experimental/text_input/text_input_event.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::textinput::detail {
namespace {

[[nodiscard]] bool
isContinuationByte(char c) noexcept
{
    return (static_cast<u8>(c) & 0xC0) == 0x80;
}

/// Describes the part of an old text that a new text replaced: the bytes from
/// start to oldEnd of the old text became the bytes from start to newEnd.
struct TextDifference final {
    u32 start = 0;
    u32 oldEnd = 0;
    u32 newEnd = 0;
};

/// Returns the shortest part of before that after replaced, widened to whole
/// code points.
[[nodiscard]] TextDifference
findDifference(std::string_view before, std::string_view after) noexcept
{
    const auto shorter = std::min(before.size(), after.size());
    std::size_t prefix = 0;
    while ((prefix < shorter) && (before[prefix] == after[prefix])) {
        ++prefix;
    }
    while ((prefix > 0) &&
           (((prefix < before.size()) && isContinuationByte(before[prefix])) ||
               ((prefix < after.size()) && isContinuationByte(after[prefix])))) {
        --prefix;
    }

    std::size_t suffix = 0;
    while ((suffix < shorter - prefix) && (before[before.size() - 1 - suffix] == after[after.size() - 1 - suffix])) {
        ++suffix;
    }
    while ((suffix > 0) && isContinuationByte(before[before.size() - suffix])) {
        --suffix;
    }

    return TextDifference{
        .start = static_cast<u32>(prefix),
        .oldEnd = static_cast<u32>(before.size() - suffix),
        .newEnd = static_cast<u32>(after.size() - suffix),
    };
}

/// Returns true when after is before with the bytes from start to end
/// replaced, so that the replacement in after begins at start.
[[nodiscard]] bool
replacesRange(std::string_view before, std::string_view after, u32 start, u32 end) noexcept
{
    const auto tail = before.size() - end;
    return (after.size() >= start + tail) &&
           (after.substr(0, start) == before.substr(0, start)) &&
           (after.substr(after.size() - tail) == before.substr(end));
}

void addSelection(std::vector<TextInputEvent>& events, u64 fieldID, u32 caret, u32 anchor)
{
    TextInputEvent event;
    event.kind = TextInputEventKind::SetSelection;
    event.fieldID = fieldID;
    event.caret = caret;
    event.anchor = anchor;
    events.push_back(std::move(event));
}

} // namespace

bool TextMirror::matches(std::string_view text, u32 caret, u32 anchor) const noexcept
{
    return (text == text_) && (caret == caret_) && (anchor == anchor_);
}

void TextMirror::reset(std::string_view text, u32 caret, u32 anchor)
{
    const auto size = static_cast<u32>(text.size());
    text_.assign(text);
    caret_ = std::min(caret, size);
    anchor_ = std::min(anchor, size);
    composing_ = false;
}

void TextMirror::applyEdit(std::string_view text, u32 caret, u32 anchor, u64 fieldID, std::vector<TextInputEvent>& events)
{
    const auto size = static_cast<u32>(text.size());
    caret = std::min(caret, size);
    anchor = std::min(anchor, size);

    // NOTE: The selection that the field has after the events so far.
    auto fieldCaret = caret_;
    auto fieldAnchor = anchor_;

    if (text != text_) {
        const auto start = std::min(caret_, anchor_);
        const auto end = std::max(caret_, anchor_);
        if (replacesRange(text_, text, start, end) && (text.size() > start + (text_.size() - end))) {
            // NOTE: Typing replaces the selection with the typed text.
            TextInputEvent commit;
            commit.kind = TextInputEventKind::Commit;
            commit.fieldID = fieldID;
            commit.text.assign(text.substr(start, text.size() - (text_.size() - end) - start));
            fieldCaret = start + static_cast<u32>(commit.text.size());
            events.push_back(std::move(commit));
        }
        else {
            // NOTE: Other edits, such as a deletion or a replaced word, delete
            // what changed before a caret placed at its end, and insert the
            // new text there.
            const auto difference = findDifference(text_, text);
            if ((caret_ != difference.oldEnd) || (anchor_ != difference.oldEnd)) {
                addSelection(events, fieldID, difference.oldEnd, difference.oldEnd);
            }
            if (difference.start < difference.oldEnd) {
                TextInputEvent deletion;
                deletion.kind = TextInputEventKind::DeleteSurrounding;
                deletion.fieldID = fieldID;
                deletion.deleteBefore = difference.oldEnd - difference.start;
                events.push_back(std::move(deletion));
            }
            if (difference.start < difference.newEnd) {
                TextInputEvent commit;
                commit.kind = TextInputEventKind::Commit;
                commit.fieldID = fieldID;
                commit.text.assign(text.substr(difference.start, difference.newEnd - difference.start));
                events.push_back(std::move(commit));
            }
            fieldCaret = difference.newEnd;
        }
        fieldAnchor = fieldCaret;
        text_.assign(text);
    }

    if ((caret != fieldCaret) || (anchor != fieldAnchor)) {
        addSelection(events, fieldID, caret, anchor);
    }
    caret_ = caret;
    anchor_ = anchor;
}

void TextMirror::applyComposition(std::string_view text, u32 caret, u32 anchor, u64 fieldID, std::vector<TextInputEvent>& events)
{
    const auto size = static_cast<u32>(text.size());
    caret = std::min(caret, size);
    anchor = std::min(anchor, size);

    auto start = std::min(caret_, anchor_);
    const auto end = std::max(caret_, anchor_);
    u32 compositionEnd = 0;
    if (replacesRange(text_, text, start, end)) {
        compositionEnd = size - static_cast<u32>(text_.size() - end);
    }
    else {
        // NOTE: The input method composes over text of the field, as Android
        // keyboards do with the word at the caret. Select that text, so that
        // the composition shows in its place and its commit replaces it.
        const auto difference = findDifference(text_, text);
        addSelection(events, fieldID, difference.oldEnd, difference.start);
        caret_ = difference.oldEnd;
        anchor_ = difference.start;
        start = difference.start;
        compositionEnd = difference.newEnd;
    }

    TextInputEvent event;
    event.kind = TextInputEventKind::CompositionUpdate;
    event.fieldID = fieldID;
    event.text.assign(text.substr(start, compositionEnd - start));

    // NOTE: A selection within the composed text marks the clause that the
    // input method converts, and the caret goes to its end.
    const auto selectionStart = std::min(caret, anchor);
    const auto selectionEnd = std::max(caret, anchor);
    event.caret = std::clamp(caret, start, compositionEnd) - start;
    if ((selectionStart < selectionEnd) && (start <= selectionStart) && (selectionEnd <= compositionEnd)) {
        event.caret = selectionEnd - start;
        event.targetStart = selectionStart - start;
        event.targetEnd = selectionEnd - start;
    }
    events.push_back(std::move(event));
    composing_ = true;
}

void TextMirror::endComposition(std::string_view text, u32 caret, u32 anchor, u64 fieldID, std::vector<TextInputEvent>& events)
{
    // NOTE: The composed text, now part of the element's text, arrives as an
    // edit, which commits it before the composition ends.
    composing_ = false;
    applyEdit(text, caret, anchor, fieldID, events);

    TextInputEvent event;
    event.kind = TextInputEventKind::CompositionEnd;
    event.fieldID = fieldID;
    events.push_back(std::move(event));
}

bool TextMirror::isComposing() const noexcept
{
    return composing_;
}

} // namespace pomdog::textinput::detail
