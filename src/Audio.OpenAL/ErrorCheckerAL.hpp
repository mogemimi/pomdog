// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesOpenAL.hpp"
#include <memory>
#include <string>

namespace Pomdog {
class Error;
} // namespace Pomdog

namespace Pomdog::Detail::OpenAL {

[[nodiscard]] std::shared_ptr<Error>
MakeOpenALError(ALenum err, std::string&& message) noexcept;

#if defined(DEBUG) && !defined(NDEBUG)
void CheckError(const char* command, const char* filename, int line) noexcept;
#endif

#if defined(DEBUG) && !defined(NDEBUG)
#define POMDOG_CHECK_ERROR_OPENAL(name) \
    Pomdog::Detail::OpenAL::CheckError(name, __FILE__, __LINE__)
#else
#define POMDOG_CHECK_ERROR_OPENAL(name)
#endif

} // namespace Pomdog::Detail::OpenAL
