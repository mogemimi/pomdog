// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::gpu::detail::gl4 {

[[nodiscard]] std::unique_ptr<Error>
GetLastError() noexcept;

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
void CheckError(const char* command, const char* filename, int line);
#endif

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
#define POMDOG_CHECK_ERROR_GL4(name) \
    pomdog::gpu::detail::gl4::CheckError(name, __FILE__, __LINE__)
#else
#define POMDOG_CHECK_ERROR_GL4(name)
#endif

} // namespace pomdog::gpu::detail::gl4
