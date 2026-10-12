// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/i18n/line_break_rules.h"

namespace pomdog::i18n {

bool isCJKCharacter(char32_t codePoint) noexcept
{
    // NOTE: CJK Unified Ideographs
    if (codePoint >= 0x4E00 && codePoint <= 0x9FFF) {
        return true;
    }
    // NOTE: CJK Unified Ideographs Extension A
    if (codePoint >= 0x3400 && codePoint <= 0x4DBF) {
        return true;
    }
    // NOTE: CJK Unified Ideographs Extension B to F
    if (codePoint >= 0x20000 && codePoint <= 0x2CEAF) {
        return true;
    }
    // NOTE: CJK Compatibility Ideographs
    if (codePoint >= 0xF900 && codePoint <= 0xFAFF) {
        return true;
    }
    // NOTE: CJK Symbols and Punctuation, such as "、", "。", and "「".
    // U+3000 IDEOGRAPHIC SPACE is breakable whitespace instead.
    if (codePoint >= 0x3001 && codePoint <= 0x303F) {
        return true;
    }
    // NOTE: Hiragana
    if (codePoint >= 0x3040 && codePoint <= 0x309F) {
        return true;
    }
    // NOTE: Katakana
    if (codePoint >= 0x30A0 && codePoint <= 0x30FF) {
        return true;
    }
    // NOTE: Katakana Phonetic Extensions
    if (codePoint >= 0x31F0 && codePoint <= 0x31FF) {
        return true;
    }
    // NOTE: Hangul Syllables
    if (codePoint >= 0xAC00 && codePoint <= 0xD7AF) {
        return true;
    }
    // NOTE: Hangul Jamo
    if (codePoint >= 0x1100 && codePoint <= 0x11FF) {
        return true;
    }
    // NOTE: Hangul Compatibility Jamo
    if (codePoint >= 0x3130 && codePoint <= 0x318F) {
        return true;
    }
    // NOTE: Full-width ASCII variants and full-width white parentheses
    if (codePoint >= 0xFF01 && codePoint <= 0xFF60) {
        return true;
    }
    // NOTE: Halfwidth CJK punctuation and halfwidth Katakana
    if (codePoint >= 0xFF61 && codePoint <= 0xFF9F) {
        return true;
    }
    return false;
}

bool isLineStartProhibited(char32_t codePoint) noexcept
{
    // NOTE: Small Katakana for Ainu
    if (codePoint >= 0x31F0 && codePoint <= 0x31FF) {
        return true;
    }
    // NOTE: Halfwidth small Katakana from U+FF67 to U+FF6F, and the halfwidth
    // prolonged sound mark U+FF70
    if (codePoint >= 0xFF67 && codePoint <= 0xFF70) {
        return true;
    }

    switch (codePoint) {
    // NOTE: Japanese and Chinese punctuation
    case U'、':     // Ideographic comma
    case U'。':     // Ideographic full stop
    case U'，':     // Full-width comma
    case U'．':     // Full-width full stop
    case U'：':     // Full-width colon
    case U'；':     // Full-width semicolon
    case U'！':     // Full-width exclamation mark
    case U'？':     // Full-width question mark
    case U'）':     // Full-width right parenthesis
    case U'］':     // Full-width right square bracket
    case U'｝':     // Full-width right curly bracket
    case U'」':     // Right corner bracket
    case U'』':     // Right white corner bracket
    case U'】':     // Right black lenticular bracket
    case U'〉':     // Right angle bracket
    case U'》':     // Right double angle bracket
    case U'〕':     // Right tortoise shell bracket
    case U'〗':     // Right white lenticular bracket
    case U'〙':     // Right white tortoise shell bracket
    case U'〛':     // Right white square bracket
    case U'\u301E': // Double prime quotation mark
    case U'\u301F': // Low double prime quotation mark
    case U'\uFF60': // Full-width right white parenthesis
    case U'\u2986': // Right white parenthesis
    case U'\uFF61': // Halfwidth ideographic full stop
    case U'\uFF63': // Halfwidth right corner bracket
    case U'\uFF64': // Halfwidth ideographic comma
    case U'\uFF65': // Halfwidth Katakana middle dot
    case U'\u203C': // Double exclamation mark
    case U'\u2047': // Double question mark
    case U'\u2048': // Question exclamation mark
    case U'\u2049': // Exclamation question mark
    case U'ー':     // Katakana-hiragana prolonged sound mark
    case U'\u301C': // Wave dash
    case U'\uFF5E': // Full-width tilde
    case U'・':     // Katakana middle dot
    case U'\u2026': // Horizontal ellipsis
    case U'\u2025': // Two dot leader

    // NOTE: Japanese hyphens
    case U'゠':     // Katakana-hiragana double hyphen
    case U'\u2010': // Hyphen
    case U'\u2013': // En dash

    // NOTE: Japanese iteration marks
    case U'々': // Ideographic iteration mark
    case U'〻': // Vertical ideographic iteration mark
    case U'ゝ': // Hiragana iteration mark
    case U'ゞ': // Hiragana voiced iteration mark
    case U'ヽ': // Katakana iteration mark
    case U'ヾ': // Katakana voiced iteration mark

    // NOTE: Japanese voiced and semi-voiced sound marks
    case U'゛':     // Katakana-hiragana voiced sound mark
    case U'゜':     // Katakana-hiragana semi-voiced sound mark
    case U'\uFF9E': // Halfwidth Katakana voiced sound mark
    case U'\uFF9F': // Halfwidth Katakana semi-voiced sound mark

    // NOTE: Japanese small kana
    case U'っ': // Hiragana small tsu
    case U'ッ': // Katakana small tsu
    case U'ぁ': // Hiragana small a
    case U'ぃ': // Hiragana small i
    case U'ぅ': // Hiragana small u
    case U'ぇ': // Hiragana small e
    case U'ぉ': // Hiragana small o
    case U'ァ': // Katakana small a
    case U'ィ': // Katakana small i
    case U'ゥ': // Katakana small u
    case U'ェ': // Katakana small e
    case U'ォ': // Katakana small o
    case U'ゃ': // Hiragana small ya
    case U'ゅ': // Hiragana small yu
    case U'ょ': // Hiragana small yo
    case U'ャ': // Katakana small ya
    case U'ュ': // Katakana small yu
    case U'ョ': // Katakana small yo
    case U'ゎ': // Hiragana small wa
    case U'ヮ': // Katakana small wa
    case U'ゕ': // Hiragana small ka
    case U'ゖ': // Hiragana small ke
    case U'ヵ': // Katakana small ka
    case U'ヶ': // Katakana small ke

    // NOTE: Closing quotation marks
    case U'\u2019': // Right single quotation mark
    case U'\u201D': // Right double quotation mark
    case U'\u203A': // Single right-pointing angle quotation mark
    case U'\u00BB': // Right-pointing double angle quotation mark (French, Russian)

    // NOTE: Chinese punctuation and symbols
    case U'\u00B7': // Middle dot (Chinese interpunct)
    case U'％':     // Full-width percent sign
    case U'℃':      // Degree Celsius
    case U'°':      // Degree sign

    // NOTE: Dashes used in Korean and Chinese text
    case U'\u2014': // Em dash
    case U'\u2015': // Horizontal bar

    // NOTE: Thai repetition and abbreviation marks
    case U'\u0E46': // Mai Yamok
    case U'\u0E2F': // Paiyannoi

    // NOTE: ASCII closing brackets and punctuation
    case U')':
    case U']':
    case U'}':
    case U',':
    case U'.':
    case U'!':
    case U'?':
    case U':':
    case U';':
    case U'%':
    case U'\'':
    case U'"':
        return true;
    default:
        return isThaiCombiningVowelOrToneMark(codePoint);
    }
}

bool isLineEndProhibited(char32_t codePoint) noexcept
{
    switch (codePoint) {
    // NOTE: Japanese and Chinese opening brackets
    case U'（':     // Full-width left parenthesis
    case U'［':     // Full-width left square bracket
    case U'｛':     // Full-width left curly bracket
    case U'「':     // Left corner bracket
    case U'『':     // Left white corner bracket
    case U'【':     // Left black lenticular bracket
    case U'〈':     // Left angle bracket
    case U'《':     // Left double angle bracket
    case U'〔':     // Left tortoise shell bracket
    case U'〖':     // Left white lenticular bracket
    case U'〘':     // Left white tortoise shell bracket
    case U'〚':     // Left white square bracket
    case U'\u301D': // Reversed double prime quotation mark
    case U'\uFF5F': // Full-width left white parenthesis
    case U'\u2985': // Left white parenthesis
    case U'\uFF62': // Halfwidth left corner bracket

    // NOTE: Opening quotation marks
    case U'\u2018': // Left single quotation mark
    case U'\u201C': // Left double quotation mark
    case U'\u2039': // Single left-pointing angle quotation mark
    case U'\u00AB': // Left-pointing double angle quotation mark (French, Russian)

    // NOTE: ASCII opening brackets
    case U'(':
    case U'[':
    case U'{':
        return true;
    default:
        return isThaiLeadingVowel(codePoint);
    }
}

bool isBreakableWhitespace(char32_t codePoint) noexcept
{
    // NOTE: U+00A0 NO-BREAK SPACE is intentionally excluded.
    switch (codePoint) {
    case U'\u0009': // Character tabulation
    case U'\u0020': // Space
    case U'\u2000': // En quad
    case U'\u2001': // Em quad
    case U'\u2002': // En space
    case U'\u2003': // Em space
    case U'\u2004': // Three-per-em space
    case U'\u2005': // Four-per-em space
    case U'\u2006': // Six-per-em space
    case U'\u2007': // Figure space
    case U'\u2008': // Punctuation space
    case U'\u2009': // Thin space
    case U'\u200A': // Hair space
    case U'\u200B': // Zero width space
    case U'\u205F': // Medium mathematical space
    case U'\u3000': // Ideographic space
        return true;
    default:
        return false;
    }
}

bool isThaiCharacter(char32_t codePoint) noexcept
{
    return codePoint >= 0x0E00 && codePoint <= 0x0E7F;
}

bool isThaiCombiningVowelOrToneMark(char32_t codePoint) noexcept
{
    // NOTE: Following vowels Sara A (U+0E30), Sara Aa (U+0E32), and Sara Am
    // (U+0E33) are written after the consonant but must stay with it.
    if (codePoint == 0x0E30 || codePoint == 0x0E32 || codePoint == 0x0E33) {
        return true;
    }
    // NOTE: Mai Han-Akat (U+0E31), an above vowel
    if (codePoint == 0x0E31) {
        return true;
    }
    // NOTE: Above and below vowels from Sara I (U+0E34) to Phinthu (U+0E3A)
    if (codePoint >= 0x0E34 && codePoint <= 0x0E3A) {
        return true;
    }
    // NOTE: Maitaikhu (U+0E47), tone marks, Thanthakhat (U+0E4C),
    // Nikhahit (U+0E4D), and Yamakkan (U+0E4E)
    if (codePoint >= 0x0E47 && codePoint <= 0x0E4E) {
        return true;
    }
    return false;
}

bool isThaiLeadingVowel(char32_t codePoint) noexcept
{
    // NOTE: Sara E, Sara Ae, Sara O, Sara Ai Maimuan, and Sara Ai Maimalai
    return codePoint >= 0x0E40 && codePoint <= 0x0E44;
}

} // namespace pomdog::i18n
