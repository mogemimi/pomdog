// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/devgui/key.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {

/// Holds the editing state of the text field that has keyboard focus.
///
/// Only one widget has keyboard focus at a time, so the context keeps one of
/// these and the focused text field reads and updates it with
/// `UI::getTextEditState()`. Offsets are byte offsets into the UTF-8 text and
/// always fall on character boundaries.
struct TextEditState final {
    /// The text field this state belongs to.
    WidgetID id = {};

    /// The caret position.
    u32 caret = 0;

    /// The end of the selection opposite the caret. It equals caret when no
    /// text is selected.
    u32 anchor = 0;

    /// How far the text is scrolled to the left to keep the caret visible.
    f32 scrollX = 0.0f;

    /// The time the caret last moved. The caret blinks relative to it, so it
    /// stays visible while the user types.
    f64 caretMovedTime = 0.0;

    /// The text when the field gained focus. Escape restores it.
    std::string originalText;

    /// The text that an input method editor is composing. It shows at the
    /// caret in place of the selection but is not part of the value until a
    /// commit inserts it.
    std::string compositionText;

    /// The caret offset in compositionText.
    u32 compositionCaret = 0;

    /// The part of compositionText that the input method is converting.
    u32 compositionTargetStart = 0;
    u32 compositionTargetEnd = 0;

    /// True from the first composition update until the composition ends,
    /// even while compositionText is empty. The input method then owns the
    /// editing keys.
    bool composing = false;

    /// The clipboard request that reads text to paste, or zero.
    u64 pasteRequest = 0;

    /// The clipboard request that writes the text being cut, or zero. The
    /// selection is erased only after the write succeeds, and only while the
    /// value and the selection are still cutValue, cutStart, and cutEnd.
    u64 cutRequest = 0;
    std::string cutValue;
    u32 cutStart = 0;
    u32 cutEnd = 0;
};

} // namespace pomdog::devgui
