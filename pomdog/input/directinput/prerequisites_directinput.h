// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/platform/win32/prerequisites_win32.h"

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
