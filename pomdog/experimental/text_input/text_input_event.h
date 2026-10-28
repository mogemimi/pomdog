// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::textinput {

/// Identifies what a `TextInputEvent` asks the text field to do.
enum class TextInputEventKind : u8 {
    /// Inserts text at the caret, replacing the selection, and removes the
    /// text being composed, if any.
    Commit,

    /// Replaces the text being composed with text. The field draws it at the
    /// caret, but it is not part of the field's value. The first update starts
    /// a composition.
    CompositionUpdate,

    /// Removes the text being composed, which ends the composition. When the
    /// composition was confirmed, a Commit with its text comes first.
    CompositionEnd,

    /// Deletes deleteBefore bytes before the selection and deleteAfter bytes
    /// after it. A replacement arrives as this event followed by a Commit.
    DeleteSurrounding,

    /// Selects the text from anchor to caret, offsets in the field's text as
    /// `TextFieldState::surroundingText` describes it, after the events before
    /// this one. A software keyboard sends it when it moves the caret itself,
    /// such as the trackpad that the space bar turns into on iOS.
    SetSelection,
};

/// Describes one change that a text input service asks the focused text field
/// to make.
///
/// Offsets and lengths are in bytes of UTF-8 text. A GUI applies the events in
/// order, only to the field whose identifier is fieldID; events for a field
/// that no longer has focus are ignored.
struct TextInputEvent final {
    TextInputEventKind kind = TextInputEventKind::Commit;

    /// The identifier of the field, from `TextFieldState::id`.
    u64 fieldID = 0;

    /// The committed text, or the whole text being composed. Text from a
    /// service contains no control characters.
    std::string text;

    /// The caret offset in the text being composed, or for SetSelection, in
    /// the field's text.
    u32 caret = 0;

    /// The other end of the selection for SetSelection, which equals caret
    /// when nothing is selected.
    u32 anchor = 0;

    /// The part of the text being composed that the input method is
    /// converting, which the GUI emphasizes. It is empty when targetStart
    /// equals targetEnd.
    u32 targetStart = 0;
    u32 targetEnd = 0;

    /// The bytes to delete before and after the selection.
    u32 deleteBefore = 0;
    u32 deleteAfter = 0;
};

} // namespace pomdog::textinput
