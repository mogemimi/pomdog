//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PREREQUISITESXAUDIO2_89DAE035_1711_4093_962F_B8CEF06B9889_HPP
#define POMDOG_PREREQUISITESXAUDIO2_89DAE035_1711_4093_962F_B8CEF06B9889_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "../Platform.Win32/Win32Prerequisites.hpp"

#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
#	// for Windows 8
#	include <xaudio2.h>
#else
#	// for Windows 7, Vista
#	ifndef _WIN32_DCOM
#		define _WIN32_DCOM
#	endif
#	include <XAudio2.h>
#endif

#endif // !defined(POMDOG_PREREQUISITESXAUDIO2_89DAE035_1711_4093_962F_B8CEF06B9889_HPP)
