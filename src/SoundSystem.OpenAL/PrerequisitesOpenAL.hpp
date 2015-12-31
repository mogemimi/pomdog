// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Platform.hpp"

#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif defined(POMDOG_PLATFORM_LINUX)
#include <AL/al.h>
#include <AL/alc.h>
#endif
