// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::clipboard::detail {

/// Appends text to result with every line break as LF.
///
/// CR LF and a lone CR each become one LF, so that text from any platform
/// reaches the caller with LF line breaks.
void appendWithLF(std::string_view text, std::string& result);

/// Appends text to result with every line break as CR LF, which Windows
/// expects on its clipboard.
///
/// CR LF, a lone CR, and a lone LF each become one CR LF.
void appendWithCRLF(std::string_view text, std::string& result);

} // namespace pomdog::clipboard::detail
