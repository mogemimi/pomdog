// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"

#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
// for Windows 8
#include <xaudio2.h>
#else
// for Windows 7, Vista
#ifndef _WIN32_DCOM
#define _WIN32_DCOM
#endif
#include <XAudio2.h>
#endif
