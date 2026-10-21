// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/text_input/utf16_conversion.h"
#include "pomdog/experimental/text_input/utf32_conversion.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::textinput::detail {
namespace {

constexpr char32_t replacementCharacter = 0xFFFD;

[[nodiscard]] bool
isHighSurrogate(char16_t c) noexcept
{
    return (c >= 0xD800) && (c <= 0xDBFF);
}

[[nodiscard]] bool
isLowSurrogate(char16_t c) noexcept
{
    return (c >= 0xDC00) && (c <= 0xDFFF);
}

/// Reads the code point at index of text and returns it with the number of
/// code units it takes.
[[nodiscard]] std::pair<char32_t, std::size_t>
readCodePoint(std::u16string_view text, std::size_t index) noexcept
{
    const auto c = text[index];
    if (isHighSurrogate(c) && (index + 1 < text.size()) && isLowSurrogate(text[index + 1])) {
        const auto high = static_cast<char32_t>(c - 0xD800);
        const auto low = static_cast<char32_t>(text[index + 1] - 0xDC00);
        return {0x10000 + ((high << 10) | low), 2};
    }
    if (isHighSurrogate(c) || isLowSurrogate(c)) {
        return {replacementCharacter, 1};
    }
    return {static_cast<char32_t>(c), 1};
}

[[nodiscard]] u32
getUTF8Length(char32_t codePoint) noexcept
{
    if (codePoint < 0x80) {
        return 1;
    }
    if (codePoint < 0x800) {
        return 2;
    }
    if (codePoint < 0x10000) {
        return 3;
    }
    return 4;
}

} // namespace

void appendUTF16AsUTF8(std::u16string_view text, std::string& result)
{
    std::size_t index = 0;
    while (index < text.size()) {
        const auto [codePoint, units] = readCodePoint(text, index);
        appendCodePointAsUTF8(codePoint, result);
        index += units;
    }
}

u32 toUTF8Offset(std::u16string_view text, std::size_t offset) noexcept
{
    u32 length = 0;
    std::size_t index = 0;
    while (index < text.size()) {
        const auto [codePoint, units] = readCodePoint(text, index);
        if (index + units > offset) {
            break;
        }
        length += getUTF8Length(codePoint);
        index += units;
    }
    return length;
}

} // namespace pomdog::textinput::detail
