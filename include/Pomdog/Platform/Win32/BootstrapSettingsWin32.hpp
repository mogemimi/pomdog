// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

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

} // namespace Win32
} // namespace Detail
} // namespace Pomdog
