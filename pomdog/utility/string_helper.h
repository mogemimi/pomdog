// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <string>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::strings {

[[nodiscard]] POMDOG_EXPORT bool
hasPrefix(std::string_view s, std::string_view prefix);

[[nodiscard]] POMDOG_EXPORT bool
hasSuffix(std::string_view s, std::string_view suffix);

[[nodiscard]] POMDOG_EXPORT std::string_view
trimRight(std::string_view source, char separator);

[[nodiscard]] POMDOG_EXPORT std::string_view
trimLeft(std::string_view source, char separator);

[[nodiscard]] POMDOG_EXPORT std::string_view
trimRight(std::string_view source, std::function<bool(char)> isSeparator);

[[nodiscard]] POMDOG_EXPORT std::string_view
trimLeft(std::string_view source, std::function<bool(char)> isSeparator);

[[nodiscard]] POMDOG_EXPORT std::vector<std::string_view>
split(std::string_view source, char separator);

[[nodiscard]] POMDOG_EXPORT std::vector<std::string_view>
split(std::string_view source, std::string_view separator);

[[nodiscard]] POMDOG_EXPORT std::string
replaceAll(std::string_view s, std::string_view from, std::string_view to);

} // namespace pomdog::strings
