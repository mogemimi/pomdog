// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include <string>
#include <vector>

namespace Pomdog {
namespace StringHelper {

POMDOG_EXPORT
std::vector<std::string>
Split(const std::string& source, char separator);

POMDOG_EXPORT
std::vector<std::string>
Split(const std::string& source, const std::string& separator);

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
