// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {
class Error;
} // namespace Pomdog

namespace Pomdog::Detail::GL4 {

[[nodiscard]] std::unique_ptr<Error>
GetLastError() noexcept;

#if defined(DEBUG) && !defined(NDEBUG)
void CheckError(const char* command, const char* filename, int line);
#endif

#if defined(DEBUG) && !defined(NDEBUG)
#define POMDOG_CHECK_ERROR_GL4(name) \
    Pomdog::Detail::GL4::CheckError(name, __FILE__, __LINE__)
#else
#define POMDOG_CHECK_ERROR_GL4(name)
#endif

} // namespace Pomdog::Detail::GL4
