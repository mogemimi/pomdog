// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"

#if defined(POMDOG_PLATFORM_MACOSX)
// NOTE: For silencing OpenAL warnings on macOS >= 10.15.
#ifndef OPENAL_DEPRECATED
#define OPENAL_DEPRECATED
#endif
#endif

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif defined(POMDOG_PLATFORM_LINUX) || defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include <AL/al.h>
#include <AL/alc.h>
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END
