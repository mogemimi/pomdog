// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#ifndef DIRECTINPUT_VERSION
#ifdef DIRECTINPUT_HEADER_VERSION
#define DIRECTINPUT_VERSION DIRECTINPUT_HEADER_VERSION
#else
#define DIRECTINPUT_VERSION 0x0800
#endif
#endif
#include <dinput.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END
