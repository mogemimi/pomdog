// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace Pomdog {
namespace StringHelper {

POMDOG_EXPORT
bool HasPrefix(const std::string_view& s, const std::string_view& prefix);

POMDOG_EXPORT
bool HasSuffix(const std::string_view& s, const std::string_view& suffix);

POMDOG_EXPORT
std::string_view TrimRight(const std::string_view& source, char separator);

POMDOG_EXPORT
std::string_view TrimLeft(const std::string_view& source, char separator);

POMDOG_EXPORT
std::string_view TrimRight(const std::string_view& source, std::function<bool(char)> isSeparator);

POMDOG_EXPORT
std::string_view TrimLeft(const std::string_view& source, std::function<bool(char)> isSeparator);

POMDOG_EXPORT
std::vector<std::string_view>
Split(const std::string_view& source, char separator);

POMDOG_EXPORT
std::vector<std::string_view>
Split(const std::string_view& source, const std::string_view& separator);

POMDOG_EXPORT
std::string
ReplaceAll(const std::string_view& s, const std::string_view& from, const std::string_view& to);

POMDOG_EXPORT
std::string Format(const char* format, ...)
#if defined(__has_attribute)
#if __has_attribute(format)
    __attribute__((__format__(printf, 1, 2)));
#endif
#elif __GNUC__ >= 4
    __attribute__((__format__(printf, 1, 2)));
#else
    ;
#endif

} // namespace StringHelper
} // namespace Pomdog
