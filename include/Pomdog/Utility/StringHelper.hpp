// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include <string>

namespace Pomdog {

struct POMDOG_EXPORT StringHelper {
    static std::string Format(const char* format, ...)
#if defined(__has_attribute)
#if __has_attribute(format)
    __attribute__((__format__(printf, 1, 2)));
#endif
#elif __GNUC__ >= 4
    __attribute__((__format__(printf, 1, 2)));
#else
    ;
#endif
};

} // namespace Pomdog
