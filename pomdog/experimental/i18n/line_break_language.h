// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::i18n {

/// Selects the line-breaking rules that wrapText applies.
///
/// The values group languages by how their lines break rather than naming
/// every display language. Applications map their own language setting to
/// one of these values.
enum class LineBreakLanguage : u8 {
    /// Breaks lines only at spaces, for space-separated languages such as
    /// English, French, German, Spanish, and Russian.
    Default,

    /// Breaks between kana and kanji, following kinsoku rules for
    /// punctuation and small kana.
    Japanese,

    /// Breaks between hanzi, for both Simplified and Traditional Chinese.
    Chinese,

    /// Breaks between Hangul syllables as well as at spaces.
    Korean,

    /// Breaks between Thai syllables found by heuristics, because Thai
    /// writes words without spaces.
    Thai,
};

} // namespace pomdog::i18n
