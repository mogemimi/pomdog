// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"

namespace Pomdog::Detail::Win32 {

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

} // namespace Pomdog::Detail::Win32
