// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/text_input/utf32_conversion.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::textinput::detail {
namespace {

constexpr char32_t replacementCharacter = 0xFFFD;

[[nodiscard]] bool
isScalarValue(char32_t codePoint) noexcept
{
    return (codePoint < 0x110000) && ((codePoint < 0xD800) || (codePoint > 0xDFFF));
}

[[nodiscard]] u32
getUTF8Length(char32_t codePoint) noexcept
{
    if (!isScalarValue(codePoint)) {
        // NOTE: U+FFFD takes three bytes.
        return 3;
    }
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

void appendCodePointAsUTF8(char32_t codePoint, std::string& result)
{
    if (!isScalarValue(codePoint)) {
        codePoint = replacementCharacter;
    }
    const auto byte = [](char32_t value) { return static_cast<char>(static_cast<u8>(value)); };
    if (codePoint < 0x80) {
        result.push_back(byte(codePoint));
    }
    else if (codePoint < 0x800) {
        result.push_back(byte(0xC0 | (codePoint >> 6)));
        result.push_back(byte(0x80 | (codePoint & 0x3F)));
    }
    else if (codePoint < 0x10000) {
        result.push_back(byte(0xE0 | (codePoint >> 12)));
        result.push_back(byte(0x80 | ((codePoint >> 6) & 0x3F)));
        result.push_back(byte(0x80 | (codePoint & 0x3F)));
    }
    else {
        result.push_back(byte(0xF0 | (codePoint >> 18)));
        result.push_back(byte(0x80 | ((codePoint >> 12) & 0x3F)));
        result.push_back(byte(0x80 | ((codePoint >> 6) & 0x3F)));
        result.push_back(byte(0x80 | (codePoint & 0x3F)));
    }
}

void appendUTF8AsUTF32(std::string_view text, std::u32string& result)
{
    std::size_t i = 0;
    while (i < text.size()) {
        const auto lead = static_cast<u8>(text[i]);
        std::size_t length = 0;
        char32_t codePoint = 0;
        char32_t minimum = 0;
        if (lead < 0x80) {
            length = 1;
            codePoint = lead;
        }
        else if ((lead & 0xE0) == 0xC0) {
            length = 2;
            codePoint = lead & 0x1F;
            minimum = 0x80;
        }
        else if ((lead & 0xF0) == 0xE0) {
            length = 3;
            codePoint = lead & 0x0F;
            minimum = 0x800;
        }
        else if ((lead & 0xF8) == 0xF0) {
            length = 4;
            codePoint = lead & 0x07;
            minimum = 0x10000;
        }

        bool valid = (length > 0) && (i + length <= text.size());
        for (std::size_t k = 1; valid && (k < length); ++k) {
            const auto continuation = static_cast<u8>(text[i + k]);
            if ((continuation & 0xC0) != 0x80) {
                valid = false;
                break;
            }
            codePoint = (codePoint << 6) | (continuation & 0x3F);
        }

        // NOTE: Reject overlong forms, surrogates, and values past U+10FFFF.
        if (valid && (codePoint >= minimum) && isScalarValue(codePoint)) {
            result.push_back(codePoint);
            i += length;
        }
        else {
            result.push_back(replacementCharacter);
            i += 1;
        }
    }
}

void appendUTF32AsUTF8(std::u32string_view text, std::string& result)
{
    for (const auto codePoint : text) {
        appendCodePointAsUTF8(codePoint, result);
    }
}

u32 toUTF8Offset(std::u32string_view text, std::size_t offset) noexcept
{
    u32 length = 0;
    for (const auto codePoint : text.substr(0, std::min(offset, text.size()))) {
        length += getUTF8Length(codePoint);
    }
    return length;
}

} // namespace pomdog::textinput::detail
