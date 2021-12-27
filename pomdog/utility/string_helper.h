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

namespace pomdog::StringHelper {

POMDOG_EXPORT
bool HasPrefix(std::string_view s, std::string_view prefix);

POMDOG_EXPORT
bool HasSuffix(std::string_view s, std::string_view suffix);

POMDOG_EXPORT
std::string_view TrimRight(std::string_view source, char separator);

POMDOG_EXPORT
std::string_view TrimLeft(std::string_view source, char separator);

POMDOG_EXPORT
std::string_view TrimRight(std::string_view source, std::function<bool(char)> isSeparator);

POMDOG_EXPORT
std::string_view TrimLeft(std::string_view source, std::function<bool(char)> isSeparator);

POMDOG_EXPORT
std::vector<std::string_view>
Split(std::string_view source, char separator);

POMDOG_EXPORT
std::vector<std::string_view>
Split(std::string_view source, std::string_view separator);

POMDOG_EXPORT
std::string
ReplaceAll(std::string_view s, std::string_view from, std::string_view to);

POMDOG_EXPORT
std::string Format(const char* format, ...)
#if defined(__has_attribute)
#if __has_attribute(format)
    __attribute__((__format__(printf, 1, 2)));
#endif
#elif defined(__GNUC__) && (__GNUC__ >= 4)
    __attribute__((__format__(printf, 1, 2)));
#else
    ;
#endif

} // namespace pomdog::StringHelper
