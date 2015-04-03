// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_BOOTSTRAPSETTINGSWIN32_F058A6BA_HPP
#define POMDOG_BOOTSTRAPSETTINGSWIN32_F058A6BA_HPP

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
    bool OpenGLEnabled = false;
};

}// namespace Win32
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_BOOTSTRAPSETTINGSWIN32_F058A6BA_HPP
