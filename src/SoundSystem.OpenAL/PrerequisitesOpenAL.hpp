// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PREREQUISITESOPENAL_0DE392F4_HPP
#define POMDOG_PREREQUISITESOPENAL_0DE392F4_HPP

#include "Pomdog/Basic/Platform.hpp"

#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif defined(POMDOG_PLATFORM_LINUX)
#// TODO
#endif

#endif // POMDOG_PREREQUISITESOPENAL_0DE392F4_HPP
