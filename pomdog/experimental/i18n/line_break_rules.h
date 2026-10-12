// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"

// NOTE: These character classes cover a subset of the Unicode Line Breaking
// Algorithm (UAX #14) that game text needs. wrapText combines them with the
// rules of each LineBreakLanguage.

namespace pomdog::i18n {

/// Reports whether codePoint is a CJK ideograph, kana, Hangul, CJK symbol or
/// punctuation, or full-width form. CJK text may break before or after it.
[[nodiscard]] POMDOG_EXPORT bool
isCJKCharacter(char32_t codePoint) noexcept;

/// Reports whether codePoint must not start a line. This covers closing
/// brackets and quotation marks, Latin and ideographic sentence punctuation,
/// Japanese small kana and prolonged sound marks, and Thai vowels and tone
/// marks that follow a consonant.
[[nodiscard]] POMDOG_EXPORT bool
isLineStartProhibited(char32_t codePoint) noexcept;

/// Reports whether codePoint must not end a line. This covers opening
/// brackets and quotation marks, and Thai vowels written before a consonant.
[[nodiscard]] POMDOG_EXPORT bool
isLineEndProhibited(char32_t codePoint) noexcept;

/// Reports whether codePoint is a space after which a line may break.
/// No-break spaces such as U+00A0 and line terminators are excluded.
[[nodiscard]] POMDOG_EXPORT bool
isBreakableWhitespace(char32_t codePoint) noexcept;

/// Reports whether codePoint is in the Thai block (U+0E00 to U+0E7F).
[[nodiscard]] POMDOG_EXPORT bool
isThaiCharacter(char32_t codePoint) noexcept;

/// Reports whether codePoint is a Thai vowel or tone mark that must stay with
/// the consonant before it.
[[nodiscard]] POMDOG_EXPORT bool
isThaiCombiningVowelOrToneMark(char32_t codePoint) noexcept;

/// Reports whether codePoint is a Thai vowel written before its consonant,
/// though pronounced after it (U+0E40 to U+0E44), so it must stay with the
/// consonant that follows.
[[nodiscard]] POMDOG_EXPORT bool
isThaiLeadingVowel(char32_t codePoint) noexcept;

} // namespace pomdog::i18n
