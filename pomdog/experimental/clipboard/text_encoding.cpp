// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/clipboard/text_encoding.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/utfcpp_headers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <iterator>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::clipboard::detail {

void appendLatin1AsUTF8(std::string_view text, std::string& result)
{
    for (const auto c : text) {
        const auto byte = static_cast<u8>(c);
        if (byte < 0x80) {
            result.push_back(c);
        }
        else {
            // NOTE: Latin-1 maps each byte to the code point of the same value.
            result.push_back(static_cast<char>(0xC0 | (byte >> 6)));
            result.push_back(static_cast<char>(0x80 | (byte & 0x3F)));
        }
    }
}

void appendValidUTF8(std::string_view text, std::string& result)
{
    // NOTE: utfcpp throws on invalid input, but replace_invalid() only
    // appends U+FFFD, which is valid, so it never throws.
    if (utf8::is_valid(text.begin(), text.end())) {
        result.append(text);
        return;
    }
    utf8::replace_invalid(text.begin(), text.end(), std::back_inserter(result));
}

} // namespace pomdog::clipboard::detail
