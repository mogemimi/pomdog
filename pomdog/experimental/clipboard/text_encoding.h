// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::clipboard::detail {

/// Appends text, which is ISO 8859-1 (Latin-1), to result as UTF-8.
///
/// X11 applications that predate UTF-8 offer their text in Latin-1.
void appendLatin1AsUTF8(std::string_view text, std::string& result);

/// Appends text, which other applications declare as UTF-8, to result with
/// each invalid sequence replaced by U+FFFD, so that result stays valid UTF-8.
void appendValidUTF8(std::string_view text, std::string& result);

} // namespace pomdog::clipboard::detail
