// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::textinput::detail {

/// Appends text, which is UTF-16, to result as UTF-8.
///
/// Platforms report input method text in UTF-16. An unpaired surrogate becomes
/// U+FFFD, so that result stays valid UTF-8.
void appendUTF16AsUTF8(std::u16string_view text, std::string& result);

/// Returns the length in UTF-8 bytes of the first offset UTF-16 code units of
/// text, as `appendUTF16AsUTF8()` converts them.
///
/// An offset between the two halves of a surrogate pair counts up to the
/// start of the pair, so that the result never splits a character.
[[nodiscard]] u32
toUTF8Offset(std::u16string_view text, std::size_t offset) noexcept;

} // namespace pomdog::textinput::detail
