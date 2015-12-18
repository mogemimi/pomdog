// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_PREREQUISITESDIRECTINPUT_98459263_HPP
#define POMDOG_PREREQUISITESDIRECTINPUT_98459263_HPP

#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"

#ifndef DIRECTINPUT_VERSION
#    ifdef DIRECTINPUT_HEADER_VERSION
#        define DIRECTINPUT_VERSION DIRECTINPUT_HEADER_VERSION
#    else
#        define DIRECTINPUT_VERSION 0x0800
#    endif
#endif
#include <dinput.h>

#endif // POMDOG_PREREQUISITESDIRECTINPUT_98459263_HPP
