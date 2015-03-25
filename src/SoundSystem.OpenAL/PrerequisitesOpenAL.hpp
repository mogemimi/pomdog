// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PREREQUISITESOPENAL_0DE392F4_D959_4DA7_8C19_20F82016DFA2_HPP
#define POMDOG_PREREQUISITESOPENAL_0DE392F4_D959_4DA7_8C19_20F82016DFA2_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Basic/Platform.hpp"

#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#	include <OpenAL/al.h>
#	include <OpenAL/alc.h>
#elif defined(POMDOG_PLATFORM_LINUX)
#	// TODO
#endif

#endif // !defined(POMDOG_PREREQUISITESOPENAL_0DE392F4_D959_4DA7_8C19_20F82016DFA2_HPP)
