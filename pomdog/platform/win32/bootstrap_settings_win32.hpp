// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/platform/win32/prerequisites_win32.hpp"

namespace pomdog::detail::win32 {

struct BootstrapSettingsWin32 final {
    HINSTANCE HInstance = nullptr;
    int CmdShow = SW_SHOWDEFAULT;
    HICON Icon = nullptr;
    HICON IconSmall = nullptr;
    int Width = 800;
    int Height = 480;
    bool IsFullscreen = false;
    bool OpenGLEnabled = false;
};

} // namespace pomdog::detail::win32
