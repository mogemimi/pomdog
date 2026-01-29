// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/openal/prerequisites_openal.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail::openal {

[[nodiscard]] std::unique_ptr<Error>
MakeOpenALError(ALenum err, std::string&& message) noexcept;

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
void CheckError(const char* command, const char* filename, int line) noexcept;
#endif

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
#define POMDOG_CHECK_ERROR_OPENAL(name) \
    pomdog::detail::openal::CheckError(name, __FILE__, __LINE__)
#else
#define POMDOG_CHECK_ERROR_OPENAL(name)
#endif

} // namespace pomdog::detail::openal
