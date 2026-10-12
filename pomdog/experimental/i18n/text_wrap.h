// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::i18n {
enum class LineBreakLanguage : u8;
} // namespace pomdog::i18n

namespace pomdog::i18n {

/// Holds text that wrapText has broken into lines.
struct POMDOG_EXPORT WrappedTextResult final {
    /// The wrapped UTF-8 text, with lines separated by '\n'.
    std::string wrappedText;

    /// The number of lines in wrappedText. Empty text counts as one line.
    i32 lineCount = 1;
};

/// Returns the width of a UTF-8 string in the unit of the caller's choice,
/// such as pixels.
using TextMeasureFunc = std::function<f32(std::string_view)>;

/// Returns text with '\n' inserted so that each line fits within maxWidth.
///
/// measureFunc receives a candidate line measured from its start, so kerning
/// and glyph bearings between words are part of the width. Lines break where
/// the rules of language allow. When no allowed break fits, such as in a long
/// URL or a word wider than maxWidth, the line breaks between characters
/// instead, avoiding kinsoku violations where possible. Every line then fits
/// unless a single character with its combining marks is wider than maxWidth.
///
/// Spaces at an inserted break and at the end of each line are removed;
/// spaces at the start of a paragraph are kept. "\n", "\r\n", and "\r" end a
/// paragraph and become "\n". Invalid UTF-8 sequences are replaced with
/// U+FFFD, so the result is always valid UTF-8.
[[nodiscard]] POMDOG_EXPORT WrappedTextResult
wrapText(
    const TextMeasureFunc& measureFunc,
    std::string_view text,
    f32 maxWidth,
    LineBreakLanguage language) noexcept;

} // namespace pomdog::i18n
