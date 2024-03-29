// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/platform.h"

#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif defined(POMDOG_PLATFORM_LINUX) || defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include <AL/al.h>
#include <AL/alc.h>
#endif
