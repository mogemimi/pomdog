// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog::Detail::GL4 {

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

} // namespace Pomdog::Detail::GL4
