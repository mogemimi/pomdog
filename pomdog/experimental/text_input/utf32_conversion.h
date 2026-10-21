// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::textinput::detail {

/// Appends codePoint to result as UTF-8. A surrogate or a value past U+10FFFF
/// becomes U+FFFD, so that result stays valid UTF-8.
void appendCodePointAsUTF8(char32_t codePoint, std::string& result);

/// Appends text, which is UTF-8, to result as code points.
///
/// Platforms such as XIM count edits of a composition in characters. Each byte
/// that does not start a valid sequence becomes U+FFFD.
void appendUTF8AsUTF32(std::string_view text, std::u32string& result);

/// Appends text, which is UTF-32, to result as UTF-8.
void appendUTF32AsUTF8(std::u32string_view text, std::string& result);

/// Returns the length in UTF-8 bytes of the first offset code points of text,
/// as `appendUTF32AsUTF8()` converts them.
[[nodiscard]] u32
toUTF8Offset(std::u32string_view text, std::size_t offset) noexcept;

} // namespace pomdog::textinput::detail
