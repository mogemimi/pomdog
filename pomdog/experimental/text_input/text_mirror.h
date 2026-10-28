// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::textinput {
struct TextInputEvent;
} // namespace pomdog::textinput

namespace pomdog::textinput::detail {

/// Holds the text and selection that a platform's text element shows for the
/// focused field, and turns the changes that the element reports into events.
///
/// Software keyboards on phones edit the text of the element they type into
/// and move its caret themselves, as with the trackpad that the space bar
/// turns into on iOS. A service copies the field into the element and calls
/// `reset()`, then passes each state that the element reports; the events make
/// the same change to the field. Offsets are in bytes of UTF-8 text, and the
/// selection runs from anchor to caret.
class TextMirror final {
public:
    /// Returns true when the element shows text with the selection from
    /// anchor to caret.
    [[nodiscard]] bool
    matches(std::string_view text, u32 caret, u32 anchor) const noexcept;

    /// Records that the element shows text with the selection from anchor to
    /// caret, as after the service copied the field into it, and that nothing
    /// is being composed.
    void
    reset(std::string_view text, u32 caret, u32 anchor);

    /// Appends to events the changes that turn the field into text with the
    /// selection from anchor to caret, which the element shows after the user
    /// edited it outside a composition.
    void
    applyEdit(std::string_view text, u32 caret, u32 anchor, u64 fieldID, std::vector<TextInputEvent>& events);

    /// Appends to events a composition update for the element showing text
    /// while an input method composes in it. The composed text stands in
    /// place of the selection, and the selection within it marks the clause
    /// being converted.
    void
    applyComposition(std::string_view text, u32 caret, u32 anchor, u64 fieldID, std::vector<TextInputEvent>& events);

    /// Appends to events the changes that end a composition, after which the
    /// element shows text with the selection from anchor to caret: the commit
    /// of the composed text, if any, and the end of the composition.
    void
    endComposition(std::string_view text, u32 caret, u32 anchor, u64 fieldID, std::vector<TextInputEvent>& events);

    /// Returns true from the first composition update until the composition
    /// ends.
    [[nodiscard]] bool
    isComposing() const noexcept;

private:
    std::string text_;
    u32 caret_ = 0;
    u32 anchor_ = 0;
    bool composing_ = false;
};

} // namespace pomdog::textinput::detail
