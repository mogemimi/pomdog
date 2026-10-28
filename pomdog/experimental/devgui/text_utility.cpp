// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/text_utility.h"
#include "pomdog/experimental/i18n/line_break_rules.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui::detail {
namespace {

struct DecodedCharacter final {
    char32_t codePoint = 0;
    u32 length = 1;
};

/// Decodes the code point at offset. An invalid sequence decodes as U+FFFD
/// one byte long, so that callers always advance.
[[nodiscard]] DecodedCharacter
decodeAt(std::string_view text, u32 offset) noexcept
{
    constexpr char32_t replacement = 0xFFFD;
    const auto lead = static_cast<u8>(text[offset]);
    if (lead < 0x80) {
        return DecodedCharacter{lead, 1};
    }

    u32 length = 0;
    char32_t codePoint = 0;
    if ((lead & 0xE0) == 0xC0) {
        length = 2;
        codePoint = lead & 0x1F;
    }
    else if ((lead & 0xF0) == 0xE0) {
        length = 3;
        codePoint = lead & 0x0F;
    }
    else if ((lead & 0xF8) == 0xF0) {
        length = 4;
        codePoint = lead & 0x07;
    }
    else {
        return DecodedCharacter{replacement, 1};
    }

    if (offset + length > text.size()) {
        return DecodedCharacter{replacement, 1};
    }
    for (u32 i = 1; i < length; ++i) {
        const auto trail = static_cast<u8>(text[offset + i]);
        if ((trail & 0xC0) != 0x80) {
            return DecodedCharacter{replacement, 1};
        }
        codePoint = (codePoint << 6) | (trail & 0x3F);
    }
    return DecodedCharacter{codePoint, length};
}

/// Identifies the characters that a word runs over.
enum class WordClass : u8 {
    /// Letters, digits, and underscores of scripts that separate words.
    Letter,
    Space,
    Hiragana,
    Katakana,
    Ideograph,

    /// Punctuation, symbols, and emoji, each of which is a word by itself.
    Other,
};

[[nodiscard]] WordClass
classifyForWords(char32_t c) noexcept
{
    if (c < 0x80) {
        if (((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')) || (c == '_')) {
            return WordClass::Letter;
        }
        return ((c == ' ') || (c == '\t')) ? WordClass::Space : WordClass::Other;
    }
    if (i18n::isBreakableWhitespace(c) || (c == 0x00A0) || (c == 0x2007) || (c == 0x202F)) {
        return WordClass::Space;
    }
    if ((c >= 0x3041) && (c <= 0x309F)) {
        return WordClass::Hiragana;
    }
    if (((c >= 0x30A1) && (c <= 0x30FF) && (c != 0x30FB)) || ((c >= 0x31F0) && (c <= 0x31FF)) || ((c >= 0xFF66) && (c <= 0xFF9F))) {
        return WordClass::Katakana;
    }
    if (((c >= 0x3400) && (c <= 0x4DBF)) || ((c >= 0x4E00) && (c <= 0x9FFF)) || ((c >= 0xF900) && (c <= 0xFAFF)) ||
        ((c >= 0x20000) && (c <= 0x3FFFF)) || ((c >= 0x3005) && (c <= 0x3007))) {
        return WordClass::Ideograph;
    }

    // NOTE: Latin-1 punctuation, general punctuation through the symbol
    // blocks, CJK and full-width punctuation, and emoji.
    if (((c >= 0x00A1) && (c <= 0x00BF)) || (c == 0x00D7) || (c == 0x00F7) || ((c >= 0x2000) && (c <= 0x2BFF)) ||
        ((c >= 0x3000) && (c <= 0x303F)) || (c == 0x30A0) || (c == 0x30FB) || ((c >= 0xFE30) && (c <= 0xFE4F)) ||
        ((c >= 0xFF01) && (c <= 0xFF0F)) || ((c >= 0xFF1A) && (c <= 0xFF20)) || ((c >= 0xFF3B) && (c <= 0xFF40)) ||
        ((c >= 0xFF5B) && (c <= 0xFF65)) || ((c >= 0x1F000) && (c <= 0x1FAFF)) || (c == 0xFFFD)) {
        return WordClass::Other;
    }
    return WordClass::Letter;
}

/// Reports whether codePoint joins the letters on both sides of it into one
/// word.
[[nodiscard]] bool
isWordJoiner(char32_t codePoint) noexcept
{
    return (codePoint == '\'') || (codePoint == '.') || (codePoint == 0x2019);
}

/// Reports whether codePoint attaches to the code point before it.
[[nodiscard]] bool
attachesToPrevious(char32_t codePoint) noexcept
{
    // NOTE: Line wrapping keeps the same marks with their character. Editing
    // also keeps the Thai vowels Sara A and Sara Aa with their consonant.
    return i18n::isClusterExtender(codePoint) || i18n::isThaiCombiningVowelOrToneMark(codePoint);
}

} // namespace

u32 nextCharacterBoundary(std::string_view text, u32 offset) noexcept
{
    const auto size = static_cast<u32>(text.size());
    if (offset >= size) {
        return size;
    }

    constexpr char32_t zeroWidthJoiner = 0x200D;
    const auto first = decodeAt(text, offset);
    offset += first.length;

    // NOTE: Two regional indicators form a flag. offset is a boundary, so a
    // regional indicator there starts a pair.
    if (i18n::isRegionalIndicator(first.codePoint) && (offset < size)) {
        if (const auto second = decodeAt(text, offset); i18n::isRegionalIndicator(second.codePoint)) {
            offset += second.length;
        }
    }

    // NOTE: A zero width joiner also pulls in the code point after it, which
    // keeps emoji sequences such as family emoji together.
    bool joined = false;
    while (offset < size) {
        const auto next = decodeAt(text, offset);
        if (!joined && !attachesToPrevious(next.codePoint)) {
            break;
        }
        joined = (next.codePoint == zeroWidthJoiner);
        offset += next.length;
    }
    return offset;
}

u32 previousCharacterBoundary(std::string_view text, u32 offset) noexcept
{
    // NOTE: Boundaries depend on the characters before them, so walk forward
    // from the start instead of decoding backward.
    u32 position = 0;
    u32 previous = 0;
    while (position < offset) {
        previous = position;
        position = nextCharacterBoundary(text, position);
    }
    return previous;
}

u32 clampCharacterBoundary(std::string_view text, u32 offset) noexcept
{
    const auto size = static_cast<u32>(text.size());
    u32 position = 0;
    while ((position < offset) && (position < size)) {
        const auto next = nextCharacterBoundary(text, position);
        if (next > offset) {
            break;
        }
        position = next;
    }
    return position;
}

u32 fitTextWithEllipsis(
    const std::function<f32(std::string_view)>& measure,
    std::string_view text,
    f32 maxWidth,
    f32 ellipsisWidth)
{
    const auto budget = maxWidth - ellipsisWidth;
    u32 fitted = 0;
    while (fitted < text.size()) {
        const auto next = nextCharacterBoundary(text, fitted);
        if (measure(text.substr(0, next)) > budget) {
            break;
        }
        fitted = next;
    }
    return fitted;
}

u32 findCharacterBoundaryAt(
    const std::function<f32(std::string_view)>& measure,
    std::string_view text,
    f32 x)
{
    // NOTE: Measuring each prefix in turn would take time quadratic in the
    // length of the text, so a binary search finds the last boundary at or
    // before x. Each measurement still covers the prefix from the start, which
    // keeps kerning exact.
    const auto size = static_cast<u32>(text.size());
    u32 low = 0;
    u32 high = size + 1;
    while ((high - low) > 1) {
        auto middle = clampCharacterBoundary(text, low + (high - low) / 2);
        if (middle <= low) {
            middle = nextCharacterBoundary(text, low);
            if (middle >= high) {
                break;
            }
        }
        if (measure(text.substr(0, middle)) <= x) {
            low = middle;
        }
        else {
            high = middle;
        }
    }
    if (low >= size) {
        return size;
    }

    // NOTE: Choose the nearer of the boundaries on either side of x.
    const auto next = nextCharacterBoundary(text, low);
    const auto lowX = (low > 0) ? measure(text.substr(0, low)) : 0.0f;
    const auto nextX = measure(text.substr(0, next));
    return (x < (lowX + nextX) * 0.5f) ? low : next;
}

u32 findCharacterAt(
    const std::function<f32(std::string_view)>& measure,
    std::string_view text,
    f32 x)
{
    const auto boundary = findCharacterBoundaryAt(measure, text, x);
    if ((boundary > 0) && ((boundary == text.size()) || (measure(text.substr(0, boundary)) > x))) {
        return previousCharacterBoundary(text, boundary);
    }
    return boundary;
}

TextRange findWordAt(std::string_view text, u32 offset)
{
    struct Character final {
        TextRange range;
        WordClass kind = WordClass::Other;
        bool joiner = false;
    };

    std::vector<Character> characters;
    const auto size = static_cast<u32>(text.size());
    for (u32 position = 0; position < size;) {
        const auto next = nextCharacterBoundary(text, position);
        const auto first = decodeAt(text, position).codePoint;
        auto kind = classifyForWords(first);

        // NOTE: The prolonged sound mark lengthens hiragana as well as katakana.
        if ((first == 0x30FC) && !characters.empty() && (characters.back().kind == WordClass::Hiragana)) {
            kind = WordClass::Hiragana;
        }
        characters.push_back(Character{
            .range = TextRange{.start = position, .end = next},
            .kind = kind,
            .joiner = isWordJoiner(first) && (next == position + 1),
        });
        position = next;
    }
    if (characters.empty()) {
        return TextRange{};
    }

    // NOTE: A joiner counts as a letter where letters surround it.
    const auto count = characters.size();
    for (std::size_t i = 1; i + 1 < count; ++i) {
        if (characters[i].joiner && (characters[i - 1].kind == WordClass::Letter) && (characters[i + 1].kind == WordClass::Letter)) {
            characters[i].kind = WordClass::Letter;
        }
    }

    std::size_t index = 0;
    while ((index + 1 < count) && (characters[index].range.end <= offset)) {
        ++index;
    }
    const auto kind = characters[index].kind;
    if (kind == WordClass::Other) {
        return characters[index].range;
    }
    auto first = index;
    while ((first > 0) && (characters[first - 1].kind == kind)) {
        --first;
    }
    auto last = index;
    while ((last + 1 < count) && (characters[last + 1].kind == kind)) {
        ++last;
    }
    return TextRange{.start = characters[first].range.start, .end = characters[last].range.end};
}

} // namespace pomdog::devgui::detail
