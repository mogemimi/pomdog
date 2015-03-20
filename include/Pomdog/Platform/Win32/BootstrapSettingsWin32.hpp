//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_BOOTSTRAPSETTINGSWIN32_F058A6BA_63F0_4807_8B2B_2EA2D8FB512A_HPP
#define POMDOG_BOOTSTRAPSETTINGSWIN32_F058A6BA_63F0_4807_8B2B_2EA2D8FB512A_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"

namespace Pomdog {
namespace Detail {
namespace Win32 {

struct BootstrapSettingsWin32 {
	HINSTANCE HInstance = nullptr;
	int CmdShow = SW_SHOWDEFAULT;
	HICON Icon = nullptr;
	HICON IconSmall = nullptr;
	int Width = 800;
	int Height = 480;
	bool IsFullscreen = false;
};

}// namespace Win32
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_BOOTSTRAPSETTINGSWIN32_F058A6BA_63F0_4807_8B2B_2EA2D8FB512A_HPP)
