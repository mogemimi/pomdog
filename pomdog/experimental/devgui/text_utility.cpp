// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/text_utility.h"
#include "pomdog/experimental/i18n/line_break_rules.h"

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

} // namespace pomdog::devgui::detail
