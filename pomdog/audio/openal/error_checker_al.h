// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/openal/prerequisites_openal.h"
#include <memory>
#include <string>

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail::openal {

[[nodiscard]] std::unique_ptr<Error>
MakeOpenALError(ALenum err, std::string&& message) noexcept;

#if defined(DEBUG) && !defined(NDEBUG)
void CheckError(const char* command, const char* filename, int line) noexcept;
#endif

#if defined(DEBUG) && !defined(NDEBUG)
#define POMDOG_CHECK_ERROR_OPENAL(name) \
    pomdog::detail::openal::CheckError(name, __FILE__, __LINE__)
#else
#define POMDOG_CHECK_ERROR_OPENAL(name)
#endif

} // namespace pomdog::detail::openal
