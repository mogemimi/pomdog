// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui::detail {

/// Holds a part of UTF-8 text, from start to end in bytes.
struct TextRange final {
    u32 start = 0;
    u32 end = 0;
};

/// Returns the offset of the character boundary after offset in UTF-8 text.
///
/// A character here is a code point together with the marks that attach to
/// it, such as Thai vowel and tone marks, combining diacritics and kana sound
/// marks, keycaps, variation selectors, and emoji skin tones, so that editing
/// never splits them. Emoji joined by zero width joiners and the two regional
/// indicators of a flag are one character too. offset must be a boundary,
/// such as one that `clampCharacterBoundary()` returns, so that flags pair up
/// from the start of the text. Invalid bytes count as one character each.
[[nodiscard]] u32
nextCharacterBoundary(std::string_view text, u32 offset) noexcept;

/// Returns the offset of the character boundary before offset in UTF-8 text.
[[nodiscard]] u32
previousCharacterBoundary(std::string_view text, u32 offset) noexcept;

/// Returns the character boundary at or before offset, clamped to text's size.
[[nodiscard]] u32
clampCharacterBoundary(std::string_view text, u32 offset) noexcept;

/// Returns the length in bytes of the longest prefix of text that, followed by
/// an ellipsis, fits within maxWidth. The prefix ends on a character boundary.
[[nodiscard]] u32
fitTextWithEllipsis(
    const std::function<f32(std::string_view)>& measure,
    std::string_view text,
    f32 maxWidth,
    f32 ellipsisWidth);

/// Returns the character boundary in text whose x position, measured from the
/// start of the text, is closest to x.
///
/// `measure` must not shrink as its text grows, so that the search measures
/// only a few prefixes even in long text.
[[nodiscard]] u32
findCharacterBoundaryAt(
    const std::function<f32(std::string_view)>& measure,
    std::string_view text,
    f32 x);

/// Returns the start of the character in text under x, measured from the
/// start of the text, or of the last character when x lies past its end.
[[nodiscard]] u32
findCharacterAt(
    const std::function<f32(std::string_view)>& measure,
    std::string_view text,
    f32 x);

/// Returns the word that contains the character starting at offset, which a
/// double click selects, or an empty range for empty text.
///
/// A word is a run of letters, digits, and underscores, of spaces, of
/// hiragana, of katakana, or of ideographs. An apostrophe or a period between
/// letters or digits joins them, as in "don't" and "3.14". Every other
/// character, such as punctuation or an emoji, is a word by itself.
[[nodiscard]] TextRange
findWordAt(std::string_view text, u32 offset);

} // namespace pomdog::devgui::detail
