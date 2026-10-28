// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/f32_rect2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::textinput {

/// Describes what a text field expects, so that the platform can choose a
/// suitable input method or software keyboard layout.
enum class TextInputPurpose : u8 {
    /// Any text. Input method editors are enabled.
    Text,

    /// A password. Input method editors are disabled, and the field's text is
    /// never passed to the platform.
    Password,

    /// An integer, typed with a numeric keyboard.
    Integer,

    /// A decimal number, typed with a numeric keyboard.
    Number,

    /// An email address.
    Email,

    /// A URL.
    URL,

    /// A search query. Software keyboards may show a search key.
    Search,
};

/// Describes the text field that has keyboard focus.
///
/// The GUI passes it to `TextInputService::setFocusedField()` in every frame
/// while the field has focus. Offsets are in bytes of UTF-8 text, and areas are
/// in the window's logical pixels with the origin at the top-left corner of
/// its client area and the y axis pointing down.
struct TextFieldState final {
    /// The GUI's identifier for the field, which must not be zero. Events for
    /// the field carry it, so that text never reaches another field.
    u64 id = 0;

    TextInputPurpose purpose = TextInputPurpose::Text;

    /// True when the field accepts line breaks.
    bool multiline = false;

    /// The area of the caret, or of the part that the input method is
    /// converting while it composes text. The input method shows its candidate
    /// window next to this area without covering it.
    F32Rect2D caretRect = {0.0f, 0.0f, 0.0f, 0.0f};

    /// The text of the field around the caret, without the text that is being
    /// composed. It may be empty, and it must be empty for a password. The view
    /// only needs to stay valid during the call that receives it.
    std::string_view surroundingText = {};

    /// The caret offset in surroundingText.
    u32 caret = 0;

    /// The other end of the selection in surroundingText, which equals caret
    /// when nothing is selected.
    u32 anchor = 0;
};

/// Describes where a text field lies, so that the platform can tell a tap on
/// it before the GUI does.
struct TextFieldArea final {
    /// The area of the field, in the window's logical pixels.
    F32Rect2D rect = {0.0f, 0.0f, 0.0f, 0.0f};

    /// What the field expects, which chooses the software keyboard layout.
    TextInputPurpose purpose = TextInputPurpose::Text;
};

} // namespace pomdog::textinput
