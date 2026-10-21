// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <span>
#include <string>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::textinput {
struct TextInputEvent;
} // namespace pomdog::textinput

namespace pomdog::textinput::detail {

/// Holds the text that an input method composes, for protocols such as XIM
/// that describe it as edits in characters, and converts it to the UTF-8
/// offsets of a composition event.
class CompositionBuffer final {
public:
    /// Removes the text and moves the caret to the start.
    void
    clear() noexcept;

    /// Replaces length characters at first with text.
    ///
    /// A character of text is part of the clause being converted when its
    /// entry in target is nonzero; target may be shorter than text, which
    /// leaves the rest unmarked. first and length are clamped to the text.
    void
    replace(std::size_t first, std::size_t length, std::u32string_view text, std::span<const u8> target);

    /// Marks the characters from first as replace() marks new text, without
    /// changing them. Marks past the end of the text are ignored.
    void
    setTarget(std::size_t first, std::span<const u8> target);

    /// Moves the caret to position, clamped to the text.
    void
    setCaret(std::size_t position) noexcept;

    /// Returns the caret position in characters.
    [[nodiscard]] std::size_t
    getCaret() const noexcept;

    /// Returns the number of characters.
    [[nodiscard]] std::size_t
    getLength() const noexcept;

    /// Sets the text, caret, and converted clause of event, with offsets in
    /// UTF-8 bytes. The clause is the first run of marked characters.
    void
    fillEvent(TextInputEvent& event) const;

private:
    std::u32string text_;
    std::vector<u8> target_;
    std::size_t caret_ = 0;
};

} // namespace pomdog::textinput::detail
