// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog {
namespace Detail {
namespace GL4 {

namespace ErrorChecker {
void CheckError(const char* command, const char* filename, int line);
}

#if defined(DEBUG) && !defined(NDEBUG)
#define POMDOG_CHECK_ERROR_GL4(name) \
    Pomdog::Detail::GL4::\
    ErrorChecker::CheckError(name, __FILE__, __LINE__)
#else
#define POMDOG_CHECK_ERROR_GL4(name)
#endif

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
