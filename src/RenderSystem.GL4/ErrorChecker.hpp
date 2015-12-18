// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_ERRORCHECKER_52C9764D_HPP
#define POMDOG_ERRORCHECKER_52C9764D_HPP

namespace Pomdog {
namespace Detail {
namespace GL4 {

namespace ErrorChecker {
void CheckError(char const* command, char const* filename, int line);
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

#endif // POMDOG_ERRORCHECKER_52C9764D_HPP
