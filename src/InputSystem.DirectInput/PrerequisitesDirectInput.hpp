// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"

#ifndef DIRECTINPUT_VERSION
#ifdef DIRECTINPUT_HEADER_VERSION
#define DIRECTINPUT_VERSION DIRECTINPUT_HEADER_VERSION
#else
#define DIRECTINPUT_VERSION 0x0800
#endif
#endif
#include <dinput.h>
