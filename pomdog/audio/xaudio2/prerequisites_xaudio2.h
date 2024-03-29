// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/platform/win32/prerequisites_win32.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
// NOTE: for Windows 8
#include <xaudio2.h>
#else
// NOTE: for Windows 7, Vista
#ifndef _WIN32_DCOM
#define _WIN32_DCOM
#endif
#include <XAudio2.h>
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END
